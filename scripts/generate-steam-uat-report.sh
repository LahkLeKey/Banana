#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPORT_DIR="${ROOT_DIR}/artifacts/uat/steam"
TIMESTAMP="$(date +%Y%m%d-%H%M%S)"
REPORT_FILE="${REPORT_DIR}/steam-uat-readiness-${TIMESTAMP}.md"
LOG_FILE="${REPORT_DIR}/steam-uat-readiness-${TIMESTAMP}.log"
STEAM_EXE_PATH="${1:-artifacts/steam-build/dev-candidate/win-unpacked/BananaEngineer.exe}"

mkdir -p "${REPORT_DIR}"

{
  echo "# Steam UAT Readiness Report"
  echo
  echo "- Timestamp: $(date -Iseconds)"
  echo "- CandidateExe: ${STEAM_EXE_PATH}"
  echo
  echo "## Automated Lanes"
} > "${REPORT_FILE}"

bash "${ROOT_DIR}/scripts/steam-uat-readiness.sh" "${STEAM_EXE_PATH}" | tee "${LOG_FILE}"

{
  echo
  echo "## Output Log"
  echo
  echo "- ${LOG_FILE}"
  echo
  echo "## Steam Library Follow-up"
  echo
  echo "1. Launch from Steam library entry."
  echo "2. Validate gameplay flow and Build Lab behavior."
  echo "3. Attach screenshots/video and mark checklist items complete."
} >> "${REPORT_FILE}"

echo "[steam-uat-report] report: ${REPORT_FILE}"
echo "[steam-uat-report] log: ${LOG_FILE}"
