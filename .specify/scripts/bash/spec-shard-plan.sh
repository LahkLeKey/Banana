#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

FEATURES_FILE=""
OUT_FILE=""
CONFLICTS_FILE=""

usage() {
  cat << 'EOF'
Usage: spec-shard-plan.sh --features-file <path> --out <path> [--conflicts-file <path>]

Build a deterministic shard plan.

Input file format:
- features-file: one feature directory path per line.
- conflicts-file (optional): one line per conflict pair as `featureA|featureB|reason`.

Output:
- JSON shard plan with parallel shards and sequential shard fallback.
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --features-file)
      FEATURES_FILE="$2"
      shift 2
      ;;
    --out)
      OUT_FILE="$2"
      shift 2
      ;;
    --conflicts-file)
      CONFLICTS_FILE="$2"
      shift 2
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "ERROR: Unknown option '$1'" >&2
      usage >&2
      exit 1
      ;;
  esac
done

if [[ -z "$FEATURES_FILE" || -z "$OUT_FILE" ]]; then
  echo "ERROR: --features-file and --out are required" >&2
  exit 1
fi

if [[ ! -f "$FEATURES_FILE" ]]; then
  echo "ERROR: features file not found: $FEATURES_FILE" >&2
  exit 1
fi

mapfile -t all_features < <(sed '/^\s*$/d' "$FEATURES_FILE" | LC_ALL=C sort)
if (( ${#all_features[@]} == 0 )); then
  echo "ERROR: features file is empty" >&2
  exit 1
fi

declare -A sequenced=()
declare -a conflict_rules=()

if [[ -n "$CONFLICTS_FILE" && -f "$CONFLICTS_FILE" ]]; then
  while IFS='|' read -r a b reason; do
    [[ -n "$a" ]] || continue

    if [[ "$a" == "SEQ" ]]; then
      [[ -n "$b" ]] || continue
      sequenced["$b"]=1
      if [[ -n "$reason" ]]; then
        conflict_rules+=("SEQ $b : $reason")
      else
        conflict_rules+=("SEQ $b")
      fi
      continue
    fi

    [[ -n "$b" ]] || continue
    sequenced["$a"]=1
    sequenced["$b"]=1
    if [[ -n "$reason" ]]; then
      conflict_rules+=("$a <-> $b : $reason")
    else
      conflict_rules+=("$a <-> $b")
    fi
  done < "$CONFLICTS_FILE"
fi

parallel_features=()
sequential_features=()
for feature in "${all_features[@]}"; do
  if [[ -n "${sequenced[$feature]:-}" ]]; then
    sequential_features+=("$feature")
  else
    parallel_features+=("$feature")
  fi
done

mkdir -p "$(dirname "$OUT_FILE")"

tmp_rules_file="$(mktemp)"
for rule in "${conflict_rules[@]}"; do
  printf '%s\n' "$rule" >> "$tmp_rules_file"
done

python3 - <<'PY' "$OUT_FILE" "${parallel_features[*]}" "${sequential_features[*]}" "$tmp_rules_file"
import json
import sys
from datetime import datetime, timezone

out_file = sys.argv[1]
parallel_raw = sys.argv[2].strip()
sequential_raw = sys.argv[3].strip()
rules_file = sys.argv[4]

parallel_features = [] if not parallel_raw else parallel_raw.split(" ")
sequential_features = [] if not sequential_raw else sequential_raw.split(" ")
conflict_rules = []
if rules_file:
    with open(rules_file, "r", encoding="utf-8") as f:
        conflict_rules = [line.strip() for line in f if line.strip()]

parallel_shards = []
for idx, feature in enumerate(parallel_features, start=1):
    parallel_shards.append({
        "shardId": f"shard-{idx}",
        "features": [feature],
        "requiredExtensions": ["git"],
    })

plan = {
    "generatedAtUtc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
    "version": "1.0.0",
    "parallelShards": parallel_shards,
    "sequentialShard": {
        "shardId": "shard-seq",
        "features": sequential_features,
        "reason": "shared-contract conflict routing" if sequential_features else "none",
    },
    "conflictRules": conflict_rules,
}

with open(out_file, "w", encoding="utf-8") as f:
    json.dump(plan, f, indent=2)
    f.write("\n")
PY

rm -f "$tmp_rules_file"

echo "Shard plan generated: $OUT_FILE"
