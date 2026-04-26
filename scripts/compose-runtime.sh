#!/usr/bin/env bash
# Spec 012 — full runtime stack.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

lane="${BANANA_CI_LANE:-compose-runtime}"
artifact_root="${BANANA_CI_ARTIFACT_ROOT:-.artifacts/compose-ci}"
profile="${BANANA_RUNTIME_PROFILE:-runtime}"
health_url="${BANANA_RUNTIME_HEALTH_URL:-}"
health_timeout_sec="${BANANA_RUNTIME_HEALTH_TIMEOUT_SEC:-60}"

lane_dir="$artifact_root/$lane"
mkdir -p "$lane_dir"

stage_file="$lane_dir/stage.txt"
reason_file="$lane_dir/reason-code.txt"
exit_file="$lane_dir/exit-code.txt"

compose_args=()
if [[ $# -gt 0 ]]; then
	compose_args+=("$@")
fi

stage="compose-up"
exit_code=0
reason_code="success"

run_profile_args=(--profile "$profile" --action up)
if [[ "${BANANA_RUNTIME_BUILD:-false}" == "true" ]]; then
	compose_args+=(--build)
fi
if [[ ${#compose_args[@]} -gt 0 ]]; then
	run_profile_args+=(-- "${compose_args[@]}")
fi

if bash scripts/compose-run-profile.sh "${run_profile_args[@]}"; then
	exit_code=0
else
	exit_code=$?
	case "$exit_code" in
		124)
			reason_code="timeout"
			;;
		126)
			reason_code="permission"
			;;
		*)
			reason_code="compose_up_failed"
			;;
	esac
fi

if (( exit_code == 0 )) && [[ -n "$health_url" ]]; then
	stage="health-check"
	if ! bash scripts/compose-profile-ready.sh --profile "$profile" --timeout-sec "$health_timeout_sec"; then
		exit_code=1
		reason_code="health_timeout"
	fi
fi

if (( exit_code == 0 )) && [[ "${BANANA_RUNTIME_VALIDATE_REPRODUCIBILITY:-false}" == "true" ]]; then
	stage="reproducibility-check"
	if ! bash scripts/validate-compose-run-profiles.sh --profile "$profile" --attempts "${BANANA_PROFILE_REPRO_ATTEMPTS:-3}"; then
		exit_code=1
		reason_code="reproducibility_failed"
	fi
fi

echo "$stage" > "$stage_file"
echo "$reason_code" > "$reason_file"
echo "$exit_code" > "$exit_file"

exit "$exit_code"
