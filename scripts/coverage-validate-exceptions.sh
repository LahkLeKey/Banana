#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: coverage-validate-exceptions.sh --exceptions <path> [--output <path>]
EOF
}

exceptions_path=""
output_path=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --exceptions)
      exceptions_path="$2"
      shift 2
      ;;
    --output)
      output_path="$2"
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

if [[ -z "$exceptions_path" ]]; then
  echo "--exceptions is required"
  usage
  exit 2
fi

if [[ ! -f "$exceptions_path" ]]; then
  if [[ -n "$output_path" ]]; then
    mkdir -p "$(dirname "$output_path")"
    cat > "$output_path" <<'EOF'
{
  "schema_version": 1,
  "valid": true,
  "active_count": 0,
  "expired_count": 0,
  "issues": [],
  "source": "missing"
}
EOF
  fi
  echo "No exception file found at $exceptions_path; treating as empty set."
  exit 0
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

"$python_bin" - "$exceptions_path" "$output_path" <<'PY'
import json
import sys
from datetime import date
from pathlib import Path

exceptions_path = Path(sys.argv[1])
output_path = Path(sys.argv[2]) if len(sys.argv) > 2 and sys.argv[2] else None

required = [
    "exception_id",
    "domain",
    "layer",
    "owner",
    "rationale",
    "remediation_plan",
    "expires_on",
]

issues = []
active = 0
expired = 0
seen_exception_ids = set()
allowed_layers = {"unit", "integration", "e2e"}

try:
    payload = json.loads(exceptions_path.read_text(encoding="utf-8"))
except Exception as exc:
    print(f"Failed to parse exceptions JSON: {exc}")
    sys.exit(1)

if not isinstance(payload, list):
    print("Exception file must be a JSON array")
    sys.exit(1)

for idx, item in enumerate(payload):
    if not isinstance(item, dict):
      issues.append(f"Entry {idx} is not an object")
      continue

    for key in required:
      if not item.get(key):
        issues.append(f"Entry {idx} missing required field '{key}'")

    exception_id = item.get("exception_id")
    if exception_id:
      if exception_id in seen_exception_ids:
        issues.append(f"Entry {idx} duplicates exception_id '{exception_id}'")
      seen_exception_ids.add(exception_id)

    layer = item.get("layer")
    if layer and layer not in allowed_layers:
      issues.append(f"Entry {idx} has invalid layer '{layer}'")

    domain = item.get("domain")
    if domain and not isinstance(domain, str):
      issues.append(f"Entry {idx} has non-string domain")

    expires_raw = item.get("expires_on")
    if not expires_raw:
      continue

    try:
      expires_on = date.fromisoformat(str(expires_raw))
    except ValueError:
      issues.append(f"Entry {idx} has invalid expires_on '{expires_raw}'")
      continue

    if expires_on < date.today():
      expired += 1
      issues.append(f"Entry {idx} has expired on {expires_raw}")
    else:
      active += 1

result = {
    "schema_version": 1,
    "valid": len(issues) == 0,
    "active_count": active,
    "expired_count": expired,
    "issues": issues,
    "source": str(exceptions_path),
}

if output_path:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")

if issues:
    print("Exception validation failed:")
    for issue in issues:
      print(f"- {issue}")
    sys.exit(1)

print("Exception validation passed.")
PY
