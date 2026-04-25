#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 4 ]]; then
  echo "Usage: $0 <summary.txt> <threshold-percent> <output-json> <gate-name>"
  exit 2
fi

SUMMARY_FILE="$1"
THRESHOLD="$2"
OUTPUT_JSON="$3"
GATE_NAME="$4"

python - "$SUMMARY_FILE" "$THRESHOLD" "$OUTPUT_JSON" "$GATE_NAME" <<'PY'
import json
import re
import sys
from pathlib import Path

summary_path = Path(sys.argv[1])
threshold = float(sys.argv[2])
output_path = Path(sys.argv[3])
gate_name = sys.argv[4]

if not summary_path.exists():
    raise SystemExit(f"Summary file does not exist: {summary_path}")

text = summary_path.read_text(encoding="utf-8", errors="replace")
match = re.search(r"line\s+coverage[^0-9]*([0-9]+(?:\.[0-9]+)?)\s*%", text, re.IGNORECASE)
if not match:
    all_percentages = re.findall(r"([0-9]+(?:\.[0-9]+)?)\s*%", text)
    if all_percentages:
        actual = float(all_percentages[0])
    else:
        raise SystemExit("Unable to locate line coverage percentage in summary file.")
else:
    actual = float(match.group(1))

passed = actual >= threshold
report = {
    "schema_version": 1,
    "gate": gate_name,
    "summary_file": str(summary_path),
    "threshold_percent": threshold,
    "actual_percent": round(actual, 4),
    "passed": passed,
}

output_path.parent.mkdir(parents=True, exist_ok=True)
output_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps(report, indent=2))

if not passed:
    raise SystemExit(1)
PY
