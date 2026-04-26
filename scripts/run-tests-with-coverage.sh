#!/usr/bin/env bash
# Spec 014 — dotnet test + Cobertura coverage merge.
set -euo pipefail
cd "$(dirname "$0")/.."

OUT="${BANANA_TEST_OUTPUT_DIR:-.artifacts/coverage}"
mkdir -p "$OUT"

emit_normalized_result() {
  local lane_id="$1"
  local domain="$2"
  local status="$3"
  local measured_percent="$4"
  local evidence_path="$5"

  bash scripts/coverage-denominator-policy.sh \
    --snapshot-output "$OUT/$lane_id/denominator-policy.json"

  normalize_args=(
    --lane-id "$lane_id"
    --domain "$domain"
    --layer unit
    --status "$status"
    --evidence-bundle-path "$evidence_path"
    --output "$OUT/$lane_id/coverage-lane-result.json"
  )

  if [[ -n "$measured_percent" ]]; then
    normalize_args+=(--measured-percent "$measured_percent")
  fi

  bash scripts/coverage-normalize-lane-result.sh "${normalize_args[@]}"
}

dotnet test Banana.sln \
  --collect:"XPlat Code Coverage" \
  --results-directory "$OUT" \
  -p:CollectCoverage=true -p:CoverletOutputFormat=cobertura

if [[ -x scripts/coverage-denominator-policy.sh && -x scripts/coverage-normalize-lane-result.sh ]]; then
  lane_id="${BANANA_COVERAGE_LANE_ID:-dotnet-unit}"
  domain="${BANANA_COVERAGE_DOMAIN:-aspnet}"
  coverage_file="$(find "$OUT" -type f -name 'coverage.cobertura.xml' | head -n 1 || true)"

  measured_percent=""
  if [[ -n "$coverage_file" ]]; then
    measured_percent="$(python3 - "$coverage_file" <<'PY'
import sys
import xml.etree.ElementTree as ET

tree = ET.parse(sys.argv[1])
root = tree.getroot()
line_rate = float(root.attrib.get("line-rate", "0"))
print(f"{line_rate * 100:.2f}")
PY
)"
  fi

  emit_normalized_result "$lane_id" "$domain" "pass" "$measured_percent" "$OUT"

  # Additional unit-domain normalization scaffolding for UI/runtime packages.
  # These statuses are intentionally environment-driven so callers can map
  # domain-specific runners into this shared script without duplicating logic.
  emit_normalized_result \
    "typescript-api-unit" \
    "typescript-api" \
    "${BANANA_UNIT_TYPESCRIPT_API_STATUS:-not-applicable}" \
    "${BANANA_UNIT_TYPESCRIPT_API_MEASURED:-}" \
    ".artifacts/typescript-api"

  emit_normalized_result \
    "react-unit" \
    "react" \
    "${BANANA_UNIT_REACT_STATUS:-not-applicable}" \
    "${BANANA_UNIT_REACT_MEASURED:-}" \
    ".artifacts/react"

  emit_normalized_result \
    "electron-unit" \
    "electron" \
    "${BANANA_UNIT_ELECTRON_STATUS:-not-applicable}" \
    "${BANANA_UNIT_ELECTRON_MEASURED:-}" \
    ".artifacts/electron"

  emit_normalized_result \
    "react-native-unit" \
    "react-native" \
    "${BANANA_UNIT_REACT_NATIVE_STATUS:-not-applicable}" \
    "${BANANA_UNIT_REACT_NATIVE_MEASURED:-}" \
    ".artifacts/react-native"

  emit_normalized_result \
    "shared-ui-unit" \
    "shared-ui" \
    "${BANANA_UNIT_SHARED_UI_STATUS:-not-applicable}" \
    "${BANANA_UNIT_SHARED_UI_MEASURED:-}" \
    ".artifacts/shared-ui"

  emit_normalized_result \
    "dotnet-integration" \
    "aspnet" \
    "${BANANA_INTEGRATION_DOTNET_STATUS:-not-applicable}" \
    "${BANANA_INTEGRATION_DOTNET_MEASURED:-}" \
    ".artifacts/coverage/integration-report"

  emit_normalized_result \
    "typescript-api-integration" \
    "typescript-api" \
    "${BANANA_INTEGRATION_TYPESCRIPT_API_STATUS:-not-applicable}" \
    "${BANANA_INTEGRATION_TYPESCRIPT_API_MEASURED:-}" \
    ".artifacts/typescript-api"
fi
