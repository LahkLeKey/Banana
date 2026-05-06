#!/usr/bin/env bash
# check-dashboard-copy-policy.sh
# Scans telemetry dashboard source files for restricted vendor-brand terminology.
# Exits 1 with diagnostics if violations are found; exits 0 when clean.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

SCAN_PATHS=(
  "src/typescript/react/src/pages"
  "src/typescript/react/src/components"
)

# Case-insensitive restricted terms (plain substrings, no regex needed)
BANNED_TERMS=(
  "grafana"
  "confluence"
  "datadog"
  "splunk"
  "kibana"
  "newrelic"
  "new relic"
  "dynatrace"
)

# Extensions to scan
EXTENSIONS=("tsx" "ts" "jsx" "js")

violations=0
violation_log=()

for scan_path in "${SCAN_PATHS[@]}"; do
  full_path="${REPO_ROOT}/${scan_path}"
  if [[ ! -d "${full_path}" ]]; then
    continue
  fi

  # Build find expression for extensions
  find_args=()
  for ext in "${EXTENSIONS[@]}"; do
    find_args+=(-o -name "*.${ext}")
  done

  while IFS= read -r file; do
    for term in "${BANNED_TERMS[@]}"; do
      # grep -i for case-insensitive match; -n for line numbers
      while IFS= read -r match_line; do
        if [[ -n "${match_line}" ]]; then
          violations=$((violations + 1))
          relative_file="${file#"${REPO_ROOT}/"}"
          violation_log+=("  ${relative_file}: ${match_line} [banned: \"${term}\"]")
        fi
      done < <(grep -in "${term}" "${file}" 2>/dev/null || true)
    done
  done < <(find "${full_path}" -type f \( -false "${find_args[@]}" \) 2>/dev/null)
done

if [[ ${violations} -gt 0 ]]; then
  echo "POLICY VIOLATION: ${violations} restricted term(s) found in dashboard copy."
  echo ""
  for line in "${violation_log[@]}"; do
    echo "${line}"
  done
  echo ""
  echo "Banned terms: ${BANNED_TERMS[*]}"
  echo "Remediation: Replace restricted vendor names with product-neutral wording."
  exit 1
fi

echo "Dashboard copy policy: clean (0 violations)."
exit 0
