#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPORT_DIR="${ROOT_DIR}/artifacts/security/steam-vendor"
TIMESTAMP="$(date +%Y%m%d-%H%M%S)"
REPORT_FILE="${REPORT_DIR}/steam-vendor-incident-evidence-${TIMESTAMP}.md"
HASH_FILE="${REPORT_DIR}/steam-vendor-incident-hashes-${TIMESTAMP}.txt"

mkdir -p "${REPORT_DIR}"

sha256_file() {
  local file_path="$1"
  if command -v sha256sum >/dev/null 2>&1; then
    sha256sum "${file_path}" | awk '{print $1}'
    return
  fi

  if command -v shasum >/dev/null 2>&1; then
    shasum -a 256 "${file_path}" | awk '{print $1}'
    return
  fi

  if command -v openssl >/dev/null 2>&1; then
    openssl dgst -sha256 "${file_path}" | awk '{print $2}'
    return
  fi

  echo "UNAVAILABLE"
}

record_artifact() {
  local label="$1"
  local rel_path="$2"
  local abs_path="${ROOT_DIR}/${rel_path}"

  if [[ -f "${abs_path}" ]]; then
    local size_bytes
    local hash
    size_bytes="$(wc -c < "${abs_path}" | tr -d ' ')"
    hash="$(sha256_file "${abs_path}")"
    echo "${label}|${rel_path}|present|${size_bytes}|${hash}" >> "${HASH_FILE}"
  else
    echo "${label}|${rel_path}|missing|0|N/A" >> "${HASH_FILE}"
  fi
}

record_generated_assets() {
  local generated_dir="${ROOT_DIR}/src/typescript/react/public/generated-assets"
  local matched=0

  if [[ -d "${generated_dir}" ]]; then
    while IFS= read -r -d '' file_path; do
      matched=1
      local rel_path
      rel_path="${file_path#${ROOT_DIR}/}"
      local label
      label="generated_asset_$(basename "${file_path}" .json)"
      record_artifact "${label}" "${rel_path}"
    done < <(find "${generated_dir}" -maxdepth 1 -type f -name 'banana-generated-*.json' -print0 | sort -z)
  fi

  if [[ "${matched}" -eq 0 ]]; then
    echo "generated_asset_none|src/typescript/react/public/generated-assets|missing|0|N/A" >> "${HASH_FILE}"
  fi
}

GIT_HEAD="$(git -C "${ROOT_DIR}" rev-parse HEAD 2>/dev/null || echo "UNKNOWN")"
GIT_BRANCH="$(git -C "${ROOT_DIR}" rev-parse --abbrev-ref HEAD 2>/dev/null || echo "UNKNOWN")"

{
  echo "label|path|status|size_bytes|sha256"
  record_artifact "wasm_engine_out_js" "out/wasm/engine.js"
  record_artifact "wasm_engine_out_wasm" "out/wasm/engine.wasm"
  record_artifact "wasm_engine_public_js" "src/typescript/react/public/wasm/engine.js"
  record_artifact "wasm_engine_public_wasm" "src/typescript/react/public/wasm/engine.wasm"
  record_artifact "wasm_engine_artifact_js" "artifacts/wasm/banana-wasm-engine.js"
  record_artifact "wasm_engine_artifact_wasm" "artifacts/wasm/banana-wasm-engine.wasm"
  record_generated_assets
} > "${HASH_FILE}"

{
  echo "# Steam Vendor Incident Evidence Pack"
  echo
  echo "- GeneratedAt: $(date -Iseconds)"
  echo "- RepoBranch: ${GIT_BRANCH}"
  echo "- RepoCommit: ${GIT_HEAD}"
  echo "- HashManifest: ${HASH_FILE}"
  echo
  echo "## Infrastructure-Focused Evidence"
  echo
  echo "This package is focused on distribution and artifact trust infrastructure."
  echo
  echo "1. Artifact provenance"
  echo "- Local artifact hashes are recorded in the hash manifest file above."
  echo "- Compare these hashes to Steam-delivered payload hashes during incident review."
  echo
  echo "2. Distribution channel mapping (Steam-side required)"
  echo "- BetaBranchName: <fill-from-steamworks>"
  echo "- ProductionBranchName: <fill-from-steamworks>"
  echo "- AffectedDepotIds: <fill-from-steamworks>"
  echo "- IncidentWindowUTC: <fill-from-steamworks>"
  echo
  echo "3. Publish governance (Steam-side required)"
  echo "- UploadActorIds: <fill-from-steamworks>"
  echo "- PublishActorIds: <fill-from-steamworks>"
  echo "- ApprovalPolicyObserved: <fill-from-steamworks>"
  echo
  echo "4. Cache/CDN timeline (Steam-side required)"
  echo "- CacheInvalidationRequestedAt: <fill-from-steamworks>"
  echo "- CacheInvalidationCompletedAt: <fill-from-steamworks>"
  echo "- EdgePropagationNotes: <fill-from-steamworks>"
  echo
  echo "5. Closure checks"
  echo "- Branch/depot isolation verified with evidence."
  echo "- Hash parity confirmed between build/upload/delivery."
  echo "- Unapproved WASM artifact load path is not reproducible."
  echo
  echo "## Next Actions"
  echo
  echo "1. Send this report and hash manifest to vendor security contact."
  echo "2. Request Steam-side manifest, actor, and cache evidence fields above."
  echo "3. Archive completed evidence package under incident tracking ticket."
} > "${REPORT_FILE}"

echo "[steam-vendor-evidence] report: ${REPORT_FILE}"
echo "[steam-vendor-evidence] hashes: ${HASH_FILE}"
