#!/usr/bin/env bash
# Spec 014 — tests profile.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

lane="${BANANA_CI_LANE:-compose-tests}"
artifact_root="${BANANA_CI_ARTIFACT_ROOT:-.artifacts/compose-ci}"
test_service="${BANANA_COMPOSE_TEST_SERVICE:-test-all}"
stage="compose-up"

lane_dir="$artifact_root/$lane"
mkdir -p "$lane_dir"

stage_file="$lane_dir/stage.txt"
reason_file="$lane_dir/reason-code.txt"
exit_file="$lane_dir/exit-code.txt"
remediation_file="$lane_dir/remediation.txt"

rm -f "$remediation_file"

compose_args=(--profile tests up --abort-on-container-exit --exit-code-from "$test_service")
if [[ "${BANANA_COMPOSE_TEST_BUILD:-true}" == "true" ]]; then
	compose_args+=(--build)
fi
compose_args+=("$test_service")

exit_code=0
if docker compose "${compose_args[@]}"; then
	exit_code=0
else
	exit_code=$?
fi

reason_code="success"
if (( exit_code != 0 )); then
	case "$exit_code" in
		1)
			reason_code="compose_test_failure"
			;;
		124)
			reason_code="timeout"
			;;
		126)
			reason_code="permission"
			cat > "$remediation_file" <<'EOF'
Compose tests lane failed with a permission-related exit code (126).

Remediation checklist:
1. Ensure scripts/compose-tests.sh and dependent scripts are executable.
2. Ensure Docker Compose can execute the selected service entrypoint.
3. Confirm shell scripts copied into containers use LF endings.
EOF
			;;
		*)
			reason_code="compose_command_failed"
			;;
	esac
fi

echo "$stage" > "$stage_file"
echo "$reason_code" > "$reason_file"
echo "$exit_code" > "$exit_file"

exit "$exit_code"
