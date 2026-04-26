#!/usr/bin/env bash
# Spec 041 — canonical compose run-profile orchestrator.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: scripts/compose-run-profile.sh --profile <name> [--action up|down|restart|ready] [--service <name>] [--detach true|false] [--] [extra docker compose args]

Examples:
  scripts/compose-run-profile.sh --profile runtime --action up
  scripts/compose-run-profile.sh --profile apps --action down
  scripts/compose-run-profile.sh --profile electron-desktop --action up --service electron-desktop
EOF
}

profile=""
action="up"
service=""
detach="true"
extra_args=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    --profile)
      profile="${2:-}"
      shift 2
      ;;
    --action)
      action="${2:-}"
      shift 2
      ;;
    --service)
      service="${2:-}"
      shift 2
      ;;
    --detach)
      detach="${2:-true}"
      shift 2
      ;;
    --)
      shift
      extra_args+=("$@")
      break
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      extra_args+=("$1")
      shift
      ;;
  esac
done

if [[ -z "$profile" ]]; then
  echo "[banana] --profile is required" >&2
  usage
  exit 2
fi

case "$profile" in
  runtime)
    compose_profiles=("runtime")
    default_service="api"
    ;;
  apps)
    compose_profiles=("apps")
    default_service="react-app"
    ;;
  tests)
    compose_profiles=("tests")
    default_service="test-all"
    ;;
  electron-smoke)
    compose_profiles=("electron")
    default_service="electron-example"
    ;;
  electron-desktop)
    compose_profiles=("electron")
    default_service="electron-desktop"
    ;;
  mobile)
    compose_profiles=("apps" "android-emulator")
    default_service="android-emulator"
    ;;
  workflows)
    compose_profiles=("workflows")
    default_service="workflow-train-not-banana-local"
    ;;
  api-fastify)
    compose_profiles=("api-fastify")
    default_service="api-fastify"
    ;;
  all)
    compose_profiles=("all")
    default_service="api"
    ;;
  *)
    echo "[banana] unsupported profile '$profile'" >&2
    exit 2
    ;;
esac

if [[ -z "$service" ]]; then
  service="$default_service"
fi

profile_args=()
for p in "${compose_profiles[@]}"; do
  profile_args+=(--profile "$p")
done

run_compose() {
  docker compose "${profile_args[@]}" "$@"
}

case "$action" in
  up)
    up_args=(up)
    if [[ "$detach" == "true" ]]; then
      up_args+=(-d)
    fi
    up_args+=("$service")
    up_args+=("${extra_args[@]}")
    run_compose "${up_args[@]}"
    ;;
  down)
    run_compose down "${extra_args[@]}"
    ;;
  restart)
    run_compose down
    up_args=(up)
    if [[ "$detach" == "true" ]]; then
      up_args+=(-d)
    fi
    up_args+=("$service")
    up_args+=("${extra_args[@]}")
    run_compose "${up_args[@]}"
    ;;
  ready)
    exec bash "$ROOT_DIR/scripts/compose-profile-ready.sh" --profile "$profile" --service "$service" "${extra_args[@]}"
    ;;
  *)
    echo "[banana] unsupported action '$action'" >&2
    exit 2
    ;;
esac
