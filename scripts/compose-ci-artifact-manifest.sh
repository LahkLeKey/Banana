#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: compose-ci-artifact-manifest.sh --lane <lane> [--artifact-name <name>] [--expected <path>]... [--optional <path>]... [--skip-reason <reason>] [--artifact-root <path>]
EOF
}

json_escape() {
  local input="$1"
  input="${input//\\/\\\\}"
  input="${input//\"/\\\"}"
  input="${input//$'\n'/\\n}"
  printf '%s' "$input"
}

write_json_array() {
  local indent="$1"
  shift
  local first=true
  echo "["
  for item in "$@"; do
    if [[ "$first" == true ]]; then
      first=false
    else
      echo ","
    fi
    printf "%s\"%s\"" "$indent" "$(json_escape "$item")"
  done
  echo
  printf "%s]" "${indent%  }"
}

create_placeholder() {
  local target="$1"

  if [[ "$target" == */ ]]; then
    mkdir -p "$target"
    local placeholder="$target/.placeholder"
    echo "optional artifact placeholder" > "$placeholder"
    echo "$placeholder"
    return
  fi

  mkdir -p "$(dirname "$target")"
  echo "optional artifact placeholder" > "$target"
  echo "$target"
}

lane=""
artifact_name=""
skip_reason=""
artifact_root="${BANANA_CI_ARTIFACT_ROOT:-.artifacts/compose-ci}"
expected_paths=()
optional_paths=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    --lane)
      lane="$2"
      shift 2
      ;;
    --artifact-name)
      artifact_name="$2"
      shift 2
      ;;
    --expected)
      expected_paths+=("$2")
      shift 2
      ;;
    --optional)
      optional_paths+=("$2")
      shift 2
      ;;
    --skip-reason)
      skip_reason="$2"
      shift 2
      ;;
    --artifact-root)
      artifact_root="$2"
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

if [[ -z "$lane" ]]; then
  echo "--lane is required"
  usage
  exit 2
fi

if [[ -z "$artifact_name" ]]; then
  artifact_name="$lane-artifacts"
fi

lane_dir="$artifact_root/$lane"
manifest_path="$lane_dir/artifact-manifest.json"
mkdir -p "$lane_dir"

produced_paths=()
missing_required=()
missing_optional=()

for path in "${expected_paths[@]}"; do
  if [[ -e "$path" ]]; then
    produced_paths+=("$path")
  else
    missing_required+=("$path")
  fi
done

for path in "${optional_paths[@]}"; do
  if [[ -e "$path" ]]; then
    produced_paths+=("$path")
  else
    missing_optional+=("$path")
    placeholder_path="$(create_placeholder "$path")"
    produced_paths+=("$placeholder_path")
  fi
done

publication_status="uploaded"
if (( ${#missing_required[@]} > 0 )); then
  publication_status="failed"
elif (( ${#missing_optional[@]} > 0 )); then
  publication_status="fallback-uploaded"
fi

if [[ -n "$skip_reason" && "$publication_status" != "failed" ]]; then
  publication_status="skipped"
fi

{
  echo "{"
  echo "  \"schema_version\": 1,"
  echo "  \"lane_name\": \"$(json_escape "$lane")\"," 
  echo "  \"artifact_name\": \"$(json_escape "$artifact_name")\"," 
  echo "  \"publication_status\": \"$(json_escape "$publication_status")\"," 
  if [[ -n "$skip_reason" ]]; then
    echo "  \"skip_reason\": \"$(json_escape "$skip_reason")\"," 
  else
    echo "  \"skip_reason\": null,"
  fi
  echo "  \"expected_paths\": "
  write_json_array "    " "${expected_paths[@]}"
  echo ","
  echo "  \"optional_paths\": "
  write_json_array "    " "${optional_paths[@]}"
  echo ","
  echo "  \"produced_paths\": "
  write_json_array "    " "${produced_paths[@]}"
  echo ","
  echo "  \"missing_required\": "
  write_json_array "    " "${missing_required[@]}"
  echo ","
  echo "  \"missing_optional\": "
  write_json_array "    " "${missing_optional[@]}"
  echo ","
  echo "  \"recorded_at_utc\": \"$(date -u +%Y-%m-%dT%H:%M:%SZ)\""
  echo "}"
} > "$manifest_path"

echo "Wrote artifact manifest to $manifest_path"
