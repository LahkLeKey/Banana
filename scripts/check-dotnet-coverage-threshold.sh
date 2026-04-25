#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 2 || $# -gt 4 ]]; then
  echo "Usage: $0 <summary-path> <threshold-percent> [diagnostics-json-path] [gate-name]" >&2
  exit 1
fi

SUMMARY_PATH="$1"
THRESHOLD_PERCENT="$2"
DIAGNOSTICS_PATH="${3:-}"
GATE_NAME="${4:-dotnet-line-rate}"

python - "$SUMMARY_PATH" "$THRESHOLD_PERCENT" "$DIAGNOSTICS_PATH" "$GATE_NAME" <<'PY'
import json
import pathlib
import re
import sys


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    raise SystemExit(1)


summary_path = pathlib.Path(sys.argv[1])
threshold_raw = sys.argv[2]
diagnostics_path_raw = sys.argv[3]
gate_name = sys.argv[4]

if not summary_path.exists():
    fail(f"Coverage summary file not found: {summary_path}")

try:
    threshold = float(threshold_raw)
except ValueError:
    fail(f"Threshold percent must be numeric. Received: {threshold_raw}")

if threshold < 0 or threshold > 100:
    fail(f"Threshold percent must be between 0 and 100. Received: {threshold_raw}")

summary_text = summary_path.read_text(encoding="utf-8", errors="replace")

line_coverage_match = re.search(r"Line coverage:\s*([0-9]+(?:\.[0-9]+)?)%", summary_text, flags=re.IGNORECASE)
if line_coverage_match is None:
    fail(f"Unable to parse line coverage from summary file: {summary_path}")

line_coverage = float(line_coverage_match.group(1))

def parse_int_field(field_name: str) -> int | None:
    match = re.search(rf"{re.escape(field_name)}:\s*([0-9]+)", summary_text, flags=re.IGNORECASE)
    if match is None:
        return None
    return int(match.group(1))

covered_lines = parse_int_field("Covered lines")
coverable_lines = parse_int_field("Coverable lines")
uncovered_lines = parse_int_field("Uncovered lines")

result = "pass" if line_coverage + 1e-9 >= threshold else "fail"
payload = {
    "gate": gate_name,
    "summaryPath": str(summary_path).replace("\\", "/"),
    "lineCoveragePercent": line_coverage,
    "thresholdPercent": threshold,
    "coveredLines": covered_lines,
    "coverableLines": coverable_lines,
    "uncoveredLines": uncovered_lines,
    "result": result,
}

if diagnostics_path_raw:
    diagnostics_path = pathlib.Path(diagnostics_path_raw)
    diagnostics_path.parent.mkdir(parents=True, exist_ok=True)
    diagnostics_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

message = (
    f"Coverage gate '{gate_name}': line-rate={line_coverage:.2f}% "
    f"threshold={threshold:.2f}% result={result}"
)

if result == "pass":
    print(message)
else:
    print(message, file=sys.stderr)
    raise SystemExit(1)
PY
