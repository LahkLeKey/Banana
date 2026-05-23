#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUTPUT_DIR="${ROOT_DIR}/artifacts/security/steam-vendor"
TIMESTAMP="$(date +%Y%m%d-%H%M%S)"
EMAIL_FILE="${OUTPUT_DIR}/steam-vendor-escalation-email-${TIMESTAMP}.md"
ATTACHMENT_FILE="${OUTPUT_DIR}/steam-vendor-attachment-manifest-${TIMESTAMP}.txt"

mkdir -p "${OUTPUT_DIR}"

GEN_OUTPUT="$(bash "${ROOT_DIR}/scripts/generate-steam-vendor-incident-evidence.sh")"
printf '%s\n' "${GEN_OUTPUT}"

REPORT_FILE="$(printf '%s\n' "${GEN_OUTPUT}" | sed -n 's/^\[steam-vendor-evidence\] report: //p' | tail -n 1)"
HASH_FILE="$(printf '%s\n' "${GEN_OUTPUT}" | sed -n 's/^\[steam-vendor-evidence\] hashes: //p' | tail -n 1)"

if [[ -z "${REPORT_FILE}" || ! -f "${REPORT_FILE}" ]]; then
  echo "[steam-vendor-escalation] ERROR: evidence report file not found" >&2
  exit 1
fi

if [[ -z "${HASH_FILE}" || ! -f "${HASH_FILE}" ]]; then
  echo "[steam-vendor-escalation] ERROR: hash manifest file not found" >&2
  exit 1
fi

cat > "${EMAIL_FILE}" <<EOF
Subject: Escalation: Accidental Publish Bypass Risk (WASM Runtime Artifact Trust)

Hello Steam Security / Steamworks Support,

We are escalating a beta-channel incident involving possible unapproved WASM runtime artifact loading.

Focus area: infrastructure and distribution trust controls.

Summary:
- Risk class: accidental publish bypass and runtime artifact trust-boundary violation.
- Concern: runtime artifact identity may drift across build, upload, and delivery paths.
- Impact: potential non-approved runtime artifact resolution.

Request for vendor-side validation:
1. Validate beta/production branch and depot isolation for AppID 4767150.
2. Confirm no cross-branch/depot artifact bleed in the incident window.
3. Provide upload/publish event trail with actor IDs and manifest/depot identifiers.
4. Provide cache/CDN invalidation and propagation timeline for affected artifacts.
5. Confirm recommended Steam controls for strict artifact identity enforcement.

Attached evidence:
- Infra incident evidence report
- Artifact hash manifest (build-side)
- Vendor incident brief and closure criteria

Thank you.

Banana Engineer Team
EOF

{
  echo "Steam Vendor Escalation Attachment Manifest"
  echo "GeneratedAt: $(date -Iseconds)"
  echo
  echo "1) ${REPORT_FILE}"
  echo "2) ${HASH_FILE}"
  echo "3) ${ROOT_DIR}/docs/steam-vendor-incident-brief.md"
  echo "4) ${ROOT_DIR}/docs/wasm-security-remediation.md"
} > "${ATTACHMENT_FILE}"

echo "[steam-vendor-escalation] email: ${EMAIL_FILE}"
echo "[steam-vendor-escalation] attachments: ${ATTACHMENT_FILE}"
