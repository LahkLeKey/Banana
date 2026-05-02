#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

BUILD_DIR="build/native-coverage"
ARTIFACT_DIR=".artifacts/native-c"
COVERAGE_ARTIFACT_ROOT="${BANANA_COVERAGE_ARTIFACT_ROOT:-.artifacts/coverage}"

if [[ -z "${BANANA_PG_CONNECTION:-}" ]]; then
    echo "BANANA_PG_CONNECTION is required for PostgreSQL-first native coverage runs."
    exit 1
fi

rm -rf "$BUILD_DIR"

cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBANANA_ENABLE_DAL=ON \
    -DBANANA_ENABLE_COVERAGE=ON

cmake --build "$BUILD_DIR"
ctest --test-dir "$BUILD_DIR" --output-on-failure

mkdir -p "$ARTIFACT_DIR"

gcovr \
    --object-directory "$BUILD_DIR" \
    --root . \
    --filter 'src/native' \
    --exclude 'tests/native' \
    --xml-pretty \
    --output "$ARTIFACT_DIR/coverage-native.xml" \
    --fail-under-line 80

gcovr \
    --object-directory "$BUILD_DIR" \
    --root . \
    --filter 'src/native' \
    --exclude 'tests/native' \
    --txt > "$ARTIFACT_DIR/Summary.txt"

echo "Native C coverage summary:"
cat "$ARTIFACT_DIR/Summary.txt"

if [[ -x scripts/coverage-denominator-policy.sh && -x scripts/coverage-normalize-lane-result.sh ]]; then
    lane_id="${BANANA_COVERAGE_LANE_ID:-native-unit}"
    measured_percent="$(python3 - "$ARTIFACT_DIR/coverage-native.xml" <<'PY'
import sys
import xml.etree.ElementTree as ET

tree = ET.parse(sys.argv[1])
root = tree.getroot()
line_rate = float(root.attrib.get("line-rate", "0"))
print(f"{line_rate * 100:.2f}")
PY
)"

    bash scripts/coverage-denominator-policy.sh \
        --snapshot-output "$COVERAGE_ARTIFACT_ROOT/$lane_id/denominator-policy.json"

    bash scripts/coverage-normalize-lane-result.sh \
        --lane-id "$lane_id" \
        --domain native \
        --layer unit \
        --status pass \
        --measured-percent "$measured_percent" \
        --evidence-bundle-path "$ARTIFACT_DIR" \
        --output "$COVERAGE_ARTIFACT_ROOT/$lane_id/coverage-lane-result.json"

    native_integration_status="${BANANA_NATIVE_INTEGRATION_STATUS:-}"
    if [[ -z "$native_integration_status" ]]; then
        native_integration_status="$(python3 - "$measured_percent" <<'PY'
import sys
print("pass" if float(sys.argv[1]) >= 80.0 else "fail")
PY
)"
    fi

    bash scripts/coverage-denominator-policy.sh \
        --snapshot-output "$COVERAGE_ARTIFACT_ROOT/native-integration/denominator-policy.json"

    bash scripts/coverage-normalize-lane-result.sh \
        --lane-id native-integration \
        --domain native \
        --layer integration \
        --status "$native_integration_status" \
        --measured-percent "$measured_percent" \
        --interop-entry-point banana_queries_search \
        --interop-entry-point banana_data_access_from_connection \
        --evidence-bundle-path "$ARTIFACT_DIR" \
        --output "$COVERAGE_ARTIFACT_ROOT/native-integration/coverage-lane-result.json"
fi
