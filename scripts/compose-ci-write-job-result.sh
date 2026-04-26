#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: compose-ci-write-job-result.sh --job <job-name> [--status <pass|fail|skip>] [--stage <stage>] [--reason-code <reason>] [--exit-code <code>] [--failing-service <service>] [--artifact-root <path>]
EOF
}

job_name=""
status=""
stage=""
reason_code=""
exit_code=""
failing_service=""
artifact_root="${BANANA_CI_ARTIFACT_ROOT:-.artifacts/compose-ci}/jobs"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --job)
      job_name="$2"
      shift 2
      ;;
    --status)
      status="$2"
      shift 2
      ;;
    --stage)
      stage="$2"
      shift 2
      ;;
    --reason-code)
      reason_code="$2"
      shift 2
      ;;
    --exit-code)
      exit_code="$2"
      shift 2
      ;;
    --failing-service)
      failing_service="$2"
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

if [[ -z "$job_name" ]]; then
  echo "--job is required"
  usage
  exit 2
fi

args=(
  --lane "$job_name"
  --artifact-root "$artifact_root"
)

if [[ -n "$status" ]]; then
  args+=(--status "$status")
fi
if [[ -n "$stage" ]]; then
  args+=(--stage "$stage")
fi
if [[ -n "$reason_code" ]]; then
  args+=(--reason-code "$reason_code")
fi
if [[ -n "$exit_code" ]]; then
  args+=(--exit-code "$exit_code")
fi
if [[ -n "$failing_service" ]]; then
  args+=(--failing-service "$failing_service")
fi

bash scripts/compose-ci-write-lane-result.sh "${args[@]}"

echo "Wrote merge-gate job result for '$job_name' under $artifact_root/$job_name"
