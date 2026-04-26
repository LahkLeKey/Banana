#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: compose-ci-aggregate-failures.sh [--search-root <path>] [--output <path>] [--text-output <path>]
EOF
}

search_root=".artifacts"
output_path=".artifacts/compose-ci/failure-aggregate-summary.json"
text_output_path=".artifacts/compose-ci/failure-aggregate-summary.txt"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --search-root)
      search_root="$2"
      shift 2
      ;;
    --output)
      output_path="$2"
      shift 2
      ;;
    --text-output)
      text_output_path="$2"
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

mapfile -t result_files < <(find "$search_root" -type f -name lane-result.json | sort)

mkdir -p "$(dirname "$output_path")"
mkdir -p "$(dirname "$text_output_path")"

python_bin=""
if command -v python3 >/dev/null 2>&1; then
  python_bin="python3"
elif command -v python >/dev/null 2>&1; then
  python_bin="python"
else
  echo "No python runtime found; cannot aggregate lane results"
  exit 1
fi

"$python_bin" - "$output_path" "$text_output_path" "$search_root" "${result_files[@]}" <<'PY'
import json
import sys
from datetime import datetime, timezone
from pathlib import Path

output_path = Path(sys.argv[1])
text_output_path = Path(sys.argv[2])
search_root = sys.argv[3]
result_files = [Path(p) for p in sys.argv[4:]]

records = []
parse_errors = []
for result_file in result_files:
    try:
        with result_file.open("r", encoding="utf-8") as f:
            payload = json.load(f)
    except Exception as exc:
        parse_errors.append({"path": str(result_file), "error": str(exc)})
        continue

    records.append(
        {
            "source_path": str(result_file),
            "name": payload.get("lane_name") or "unknown",
            "status": payload.get("status") or "unknown",
            "stage": payload.get("stage") or "unknown",
            "reason_code": payload.get("reason_code") or "unknown",
            "exit_code": payload.get("exit_code", 1),
            "recorded_at_utc": payload.get("recorded_at_utc"),
        }
    )

failed = [r for r in records if r["status"] == "fail"]
skipped = [r for r in records if r["status"] == "skip"]
passed = [r for r in records if r["status"] == "pass"]

summary = {
    "schema_version": 1,
    "generated_at_utc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
    "search_root": search_root,
    "total_results": len(records),
    "failed_count": len(failed),
    "skipped_count": len(skipped),
    "passed_count": len(passed),
    "parse_errors": parse_errors,
    "failed_results": failed,
    "skipped_results": skipped,
}

output_path.write_text(json.dumps(summary, indent=2), encoding="utf-8")

lines = []
lines.append("Compose CI Failure Aggregate")
lines.append("")
lines.append(f"generated_at_utc={summary['generated_at_utc']}")
lines.append(f"search_root={search_root}")
lines.append(f"total_results={len(records)}")
lines.append(f"failed_count={len(failed)}")
lines.append(f"skipped_count={len(skipped)}")
lines.append(f"passed_count={len(passed)}")
if parse_errors:
    lines.append("")
    lines.append("parse_errors:")
    for err in parse_errors:
        lines.append(f"- {err['path']}: {err['error']}")
if failed:
    lines.append("")
    lines.append("failed_results:")
    for record in failed:
        lines.append(
            f"- {record['name']}: stage={record['stage']} reason={record['reason_code']} exit={record['exit_code']}"
        )

text_output_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
PY

echo "Wrote failure aggregate summary to $output_path"
echo "Wrote failure aggregate text report to $text_output_path"
