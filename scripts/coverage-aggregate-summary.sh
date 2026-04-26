#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: coverage-aggregate-summary.sh --search-root <path> --output <path> [--text-output <path>]
EOF
}

search_root=""
output=""
text_output=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --search-root)
      search_root="$2"
      shift 2
      ;;
    --output)
      output="$2"
      shift 2
      ;;
    --text-output)
      text_output="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown argument: $1"
      usage
      exit 2
      ;;
  esac
done

if [[ -z "$search_root" || -z "$output" ]]; then
  echo "--search-root and --output are required"
  usage
  exit 2
fi

python_bin=""
if command -v python3 >/dev/null 2>&1; then
  python_bin="python3"
elif command -v python >/dev/null 2>&1; then
  python_bin="python"
else
  echo "python runtime not found"
  exit 1
fi

mkdir -p "$(dirname "$output")"
if [[ -n "$text_output" ]]; then
  mkdir -p "$(dirname "$text_output")"
fi

"$python_bin" - "$search_root" "$output" "$text_output" <<'PY'
import json
import sys
from collections import defaultdict
from datetime import datetime, timezone
from pathlib import Path

search_root = Path(sys.argv[1])
output = Path(sys.argv[2])
text_output = Path(sys.argv[3]) if len(sys.argv) > 3 and sys.argv[3] else None

files = sorted(search_root.rglob("coverage-lane-result.json"))

if not files:
    result = {
        "schema_version": 1,
        "run_id": "unknown",
        "commit_sha": "unknown",
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "tuple_statuses": [],
        "failed_tuple_count": 0,
        "failure_counts_by_layer": {"unit": 0, "integration": 0, "e2e": 0},
        "overall_status": "pass",
        "notes": ["No normalized coverage lane results were discovered under search root."],
    }
    output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    if text_output:
        text_output.write_text(
            "Coverage Aggregate Summary\n"
            "- No normalized coverage lane results were discovered under search root.\n",
            encoding="utf-8",
        )
    print(f"Wrote empty aggregate summary to {output}")
    sys.exit(0)

lane_rows = []
for file in files:
    payload = json.loads(file.read_text(encoding="utf-8"))
    lane_rows.append(payload)

active_exception_lanes = []
total_active_exceptions = 0
for row in lane_rows:
  exceptions = row.get("exceptions") if isinstance(row, dict) else None
  if isinstance(exceptions, dict):
    active_count = int(exceptions.get("active_count", 0) or 0)
    if active_count > 0:
      total_active_exceptions += active_count
      active_exception_lanes.append(row.get("lane_id", "unknown"))

status_rank = {"fail": 3, "pass": 2, "skip": 1, "not-applicable": 0}

tuple_map = {}
for row in lane_rows:
    domain = row.get("domain", "unknown")
    layer = row.get("layer", "unknown")
    key = (domain, layer)
    existing = tuple_map.get(key)
    row_status = row.get("status", "fail")

    if not existing or status_rank.get(row_status, 3) > status_rank.get(existing["status"], 3):
        tuple_map[key] = {
            "domain": domain,
            "layer": layer,
            "applicable": row_status != "not-applicable",
            "status": row_status,
            "measured_percent": row.get("measured_percent"),
            "threshold_percent": row.get("threshold_percent"),
            "failing_lanes": [row.get("lane_id")] if row_status == "fail" else [],
            "rationale": "policy-not-applicable" if row_status == "not-applicable" else None,
        }
    elif row_status == "fail":
        tuple_map[key]["status"] = "fail"
        tuple_map[key].setdefault("failing_lanes", []).append(row.get("lane_id"))

required_tuples = [
    ("native", "integration"),
    ("aspnet", "integration"),
    ("typescript-api", "integration"),
    ("aspnet", "e2e"),
    ("electron", "e2e"),
    ("react-native", "e2e"),
]

for domain, layer in required_tuples:
    key = (domain, layer)
    if key not in tuple_map:
        tuple_map[key] = {
            "domain": domain,
            "layer": layer,
            "applicable": True,
            "status": "fail",
            "measured_percent": None,
            "threshold_percent": 80,
            "failing_lanes": [],
            "rationale": "missing_normalized_lane_result",
        }

rows = list(tuple_map.values())
rows.sort(key=lambda r: (0 if r["status"] == "fail" else 1, r["domain"], r["layer"]))

failure_counts_by_layer = defaultdict(int)
for row in rows:
    if row["status"] == "fail":
        failure_counts_by_layer[row["layer"]] += 1

failed_tuple_count = sum(1 for row in rows if row["status"] == "fail")
overall_status = "fail" if failed_tuple_count else "pass"

result = {
    "schema_version": 1,
    "run_id": "unknown",
    "commit_sha": "unknown",
    "generated_at": datetime.now(timezone.utc).isoformat(),
    "tuple_statuses": rows,
    "failed_tuple_count": failed_tuple_count,
    "failure_counts_by_layer": {
        "unit": failure_counts_by_layer.get("unit", 0),
        "integration": failure_counts_by_layer.get("integration", 0),
        "e2e": failure_counts_by_layer.get("e2e", 0),
    },
    "overall_status": overall_status,
    "exceptions": {
      "total_active": total_active_exceptions,
      "lanes_with_active_exceptions": sorted(set(active_exception_lanes)),
    },
}

output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")

if text_output:
    lines = [
        "Coverage Aggregate Summary",
        f"- Overall status: {overall_status}",
        f"- Failed tuples: {failed_tuple_count}",
        f"- Failure counts by layer: unit={result['failure_counts_by_layer']['unit']}, integration={result['failure_counts_by_layer']['integration']}, e2e={result['failure_counts_by_layer']['e2e']}",
        f"- Active exceptions: {result['exceptions']['total_active']}",
        "",
        "Tuple Statuses:",
    ]
    for row in rows:
        measured = row.get("measured_percent")
        threshold = row.get("threshold_percent")
        lines.append(
            f"- {row['domain']}/{row['layer']}: status={row['status']} measured={measured} threshold={threshold}"
        )
    text_output.write_text("\n".join(lines) + "\n", encoding="utf-8")

print(f"Wrote aggregate summary to {output}")
PY
