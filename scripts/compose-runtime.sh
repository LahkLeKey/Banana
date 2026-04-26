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

compose_args=(--profile "$profile" up -d)
if [[ "${BANANA_RUNTIME_BUILD:-false}" == "true" ]]; then
	compose_args=(--profile "$profile" up --build -d)
fi

if [[ $# -gt 0 ]]; then
	compose_args+=("$@")
fi

stage="compose-up"
exit_code=0
reason_code="success"

if ! docker compose "${compose_args[@]}"; then
	exit_code=$?
	reason_code="compose_up_failed"
fi

if (( exit_code == 0 )) && [[ -n "$health_url" ]]; then
	stage="health-check"
	deadline=$((SECONDS + health_timeout_sec))
	until curl -fsS "$health_url" >/dev/null 2>&1; do
		if (( SECONDS >= deadline )); then
			exit_code=1
			reason_code="health_timeout"
			break
		fi
		sleep 2
	done
fi

echo "$stage" > "$stage_file"
echo "$reason_code" > "$reason_file"
echo "$exit_code" > "$exit_file"

exit "$exit_code"
