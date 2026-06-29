#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MANIFEST_PATH="${ROOT_DIR}/src/native/docs/native-module-dag.manifest"
BASELINE_PATH="${ROOT_DIR}/src/native/docs/native-module-boundary-baseline.txt"
STRICT=true

for arg in "$@"; do
  case "${arg}" in
    --strict)
      STRICT=true
      ;;
    --baseline)
      STRICT=false
      ;;
    *)
      echo "[native-module-boundaries] unknown argument: ${arg}" >&2
      echo "usage: bash scripts/validate-native-module-boundaries.sh [--strict|--baseline]" >&2
      exit 1
      ;;
  esac
done

if [[ ! -f "${MANIFEST_PATH}" ]]; then
  echo "[native-module-boundaries] missing manifest: ${MANIFEST_PATH}" >&2
  exit 1
fi

declare -A MODULES
declare -A MODULE_DEPS
PARSE_ERROR=0

while IFS= read -r raw_line; do
  line="${raw_line%%#*}"
  line="$(echo "${line}" | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')"
  [[ -z "${line}" ]] && continue

  if [[ "${line}" != *:* ]]; then
    echo "[native-module-boundaries] invalid manifest row (missing colon): ${raw_line}" >&2
    PARSE_ERROR=1
    continue
  fi

  module_name="$(echo "${line%%:*}" | sed 's/[[:space:]]*$//')"
  deps="$(echo "${line#*:}" | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')"

  if [[ -z "${module_name}" ]]; then
    echo "[native-module-boundaries] invalid manifest row (empty module): ${raw_line}" >&2
    PARSE_ERROR=1
    continue
  fi

  MODULES["${module_name}"]=1
  MODULE_DEPS["${module_name}"]="${deps}"
done < "${MANIFEST_PATH}"

if [[ ${PARSE_ERROR} -ne 0 ]]; then
  exit 1
fi

edges_tmp="$(mktemp)"
missing_tmp="$(mktemp)"
findings_tmp="$(mktemp)"
sorted_findings_tmp="$(mktemp)"
cleanup() {
  rm -f "${edges_tmp}" "${missing_tmp}" "${findings_tmp}" "${sorted_findings_tmp}"
}
trap cleanup EXIT

for module_name in "${!MODULE_DEPS[@]}"; do
  deps="${MODULE_DEPS["${module_name}"]}"
  for dep in ${deps}; do
    if [[ -z "${MODULES["${dep}"]+x}" ]]; then
      echo "[native-module-boundaries] undefined dependency '${dep}' referenced by '${module_name}'" >> "${missing_tmp}"
      continue
    fi
    if [[ "${dep}" == "${module_name}" ]]; then
      echo "[native-module-boundaries] self-cycle '${module_name}' -> '${dep}'" >> "${missing_tmp}"
      continue
    fi
    # tsort edge format: dependency then dependent.
    echo "${dep} ${module_name}" >> "${edges_tmp}"
  done
done

if [[ -s "${missing_tmp}" ]]; then
  cat "${missing_tmp}" >&2
  exit 1
fi

if [[ -s "${edges_tmp}" ]]; then
  if ! tsort "${edges_tmp}" >/dev/null 2>&1; then
    echo "[native-module-boundaries] dependency cycle detected in ${MANIFEST_PATH}" >&2
    tsort "${edges_tmp}" >/dev/null
    exit 1
  fi
fi

while IFS= read -r cmake_file; do
  rel_file="${cmake_file#${ROOT_DIR}/}"

  while IFS= read -r hit; do
    [[ -z "${hit}" ]] && continue
    detail="$(echo "${hit}" | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')"
    echo "cmake-internal-include-path|${rel_file}|${detail}" >> "${findings_tmp}"
  done < <(grep -E "modules/.*/native/src" "${cmake_file}" | grep -v "^#" | grep -Ev "set\\([A-Za-z0-9_]*SRC_DIR" || true)

  while IFS= read -r hit; do
    [[ -z "${hit}" ]] && continue
    detail="$(echo "${hit}" | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')"
    echo "cmake-internal-source-reference|${rel_file}|${detail}" >> "${findings_tmp}"
  done < <(grep -E "set\([A-Za-z0-9_]*SRC_DIR\s+\".*modules/.*/native/src\"\)" "${cmake_file}" || true)
done < <(find "${ROOT_DIR}" -type f -name "CMakeLists.txt" ! -path "${ROOT_DIR}/modules/*" ! -path "${ROOT_DIR}/.tmp-merge-stack/*")

while IFS= read -r source_file; do
  rel_file="${source_file#${ROOT_DIR}/}"
  while IFS= read -r hit; do
    [[ -z "${hit}" ]] && continue
    detail="$(echo "${hit}" | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')"
    echo "source-internal-include|${rel_file}|${detail}" >> "${findings_tmp}"
  done < <(grep -E '#include\s+["<].*modules/.*/native/src/' "${source_file}" || true)
done < <(find "${ROOT_DIR}/src/native" "${ROOT_DIR}/tests/native" -type f \( -name "*.c" -o -name "*.h" \))

sort -u "${findings_tmp}" > "${sorted_findings_tmp}"

if [[ "${STRICT}" == "true" ]]; then
  if [[ -s "${sorted_findings_tmp}" ]]; then
    echo "[native-module-boundaries] strict mode violations:" >&2
    cat "${sorted_findings_tmp}" >&2
    exit 1
  fi
  echo "[native-module-boundaries] strict mode passed: no violations found"
  exit 0
fi

sorted_baseline_tmp="$(mktemp)"
new_findings_tmp="$(mktemp)"
resolved_findings_tmp="$(mktemp)"
trap 'cleanup; rm -f "${sorted_baseline_tmp}" "${new_findings_tmp}" "${resolved_findings_tmp}"' EXIT

if [[ -f "${BASELINE_PATH}" ]]; then
  sort -u "${BASELINE_PATH}" > "${sorted_baseline_tmp}"
else
  : > "${sorted_baseline_tmp}"
fi
comm -23 "${sorted_findings_tmp}" "${sorted_baseline_tmp}" > "${new_findings_tmp}" || true
comm -13 "${sorted_findings_tmp}" "${sorted_baseline_tmp}" > "${resolved_findings_tmp}" || true

if [[ -s "${resolved_findings_tmp}" ]]; then
  echo "[native-module-boundaries] note: baseline entries resolved (consider pruning baseline):"
  cat "${resolved_findings_tmp}"
fi

if [[ -s "${new_findings_tmp}" ]]; then
  echo "[native-module-boundaries] new violations detected:" >&2
  cat "${new_findings_tmp}" >&2
  echo "[native-module-boundaries] fix violations or update baseline intentionally" >&2
  exit 1
fi

echo "[native-module-boundaries] passed: no violations beyond baseline"
