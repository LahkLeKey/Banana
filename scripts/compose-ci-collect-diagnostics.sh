#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: compose-ci-collect-diagnostics.sh --lane <lane> [--profile <compose-profile>] [--service <service>]... [--artifact-root <path>]
EOF
}

lane=""
profile=""
artifact_root="${BANANA_CI_ARTIFACT_ROOT:-.artifacts/compose-ci}"
services=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    --lane)
      lane="$2"
      shift 2
      ;;
    --profile)
      profile="$2"
      shift 2
      ;;
    --service)
      services+=("$2")
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

lane_dir="$artifact_root/$lane"
diagnostics_dir="$lane_dir/diagnostics"
mkdir -p "$diagnostics_dir"

{
  echo "lane=$lane"
  echo "profile=${profile:-n/a}"
  echo "timestamp_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
} > "$diagnostics_dir/metadata.txt"

if [[ -n "$profile" ]]; then
  docker compose --profile "$profile" ps > "$diagnostics_dir/compose-ps.txt" 2>&1 || true

  if (( ${#services[@]} == 0 )); then
    docker compose --profile "$profile" logs --no-color > "$diagnostics_dir/compose.log" 2>&1 || true
  else
    for service in "${services[@]}"; do
      docker compose --profile "$profile" logs --no-color "$service" > "$diagnostics_dir/$service.log" 2>&1 || true
    done
  fi
else
  docker ps > "$diagnostics_dir/docker-ps.txt" 2>&1 || true
fi

for metadata_file in stage.txt reason-code.txt exit-code.txt status.txt lane-result.json; do
  if [[ -f "$lane_dir/$metadata_file" ]]; then
    cp "$lane_dir/$metadata_file" "$diagnostics_dir/$metadata_file"
  fi
done

echo "Diagnostics collected at $diagnostics_dir"
