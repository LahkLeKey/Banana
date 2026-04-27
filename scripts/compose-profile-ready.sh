#!/usr/bin/env bash
# Spec 041 — deterministic compose profile readiness checks.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: scripts/compose-profile-ready.sh --profile <name> [--service <name>] [--timeout-sec <n>] [--interval-sec <n>]
EOF
}

profile=""
service=""
service_explicit="false"
timeout_sec="${BANANA_PROFILE_READY_TIMEOUT_SEC:-90}"
interval_sec="${BANANA_PROFILE_READY_INTERVAL_SEC:-2}"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --profile)
      profile="${2:-}"
      shift 2
      ;;
    --service)
      service="${2:-}"
      service_explicit="true"
      shift 2
      ;;
    --timeout-sec)
      timeout_sec="${2:-}"
      shift 2
      ;;
    --interval-sec)
      interval_sec="${2:-}"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "[banana] unknown argument: $1" >&2
      usage
      exit 2
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
    target_service="${service:-api}"
    health_url="${BANANA_RUNTIME_HEALTH_URL:-http://localhost:8080/health}"
    ;;
  apps)
    target_service="${service:-react-app}"
    health_url="${BANANA_APPS_HEALTH_URL:-http://localhost:5173}"
    companion_service=""
    companion_health_url=""
    if [[ "$service_explicit" != "true" ]]; then
      companion_service="api-fastify"
      companion_health_url="${BANANA_FASTIFY_HEALTH_URL:-http://localhost:8081/health}"
    elif [[ "$target_service" == "api-fastify" ]]; then
      health_url="${BANANA_FASTIFY_HEALTH_URL:-http://localhost:8081/health}"
    fi
    ;;
  tests)
    target_service="${service:-test-all}"
    health_url=""
    ;;
  electron-smoke)
    target_service="${service:-electron-example}"
    health_url=""
    ;;
  electron-desktop)
    target_service="${service:-electron-desktop}"
    health_url=""
    ;;
  mobile)
    target_service="${service:-android-emulator}"
    health_url="${BANANA_MOBILE_HEALTH_URL:-http://localhost:6080}"
    ;;
  workflows)
    target_service="${service:-workflow-train-not-banana-local}"
    health_url=""
    ;;
  api-fastify)
    target_service="${service:-api-fastify}"
    health_url="${BANANA_FASTIFY_HEALTH_URL:-http://localhost:8081/health}"
    ;;
  all)
    target_service="${service:-api}"
    health_url="${BANANA_RUNTIME_HEALTH_URL:-http://localhost:8080/health}"
    ;;
  *)
    echo "[banana] unsupported profile '$profile'" >&2
    exit 2
    ;;
esac

echo "[banana] waiting for profile '$profile' service '$target_service'"

deadline=$((SECONDS + timeout_sec))
while (( SECONDS < deadline )); do
  target_up="false"
  target_healthy="false"
  companion_up="true"
  companion_healthy="true"

  if docker compose ps "$target_service" 2>/dev/null | tail -n +2 | grep -qE "(Up|running|healthy)"; then
    target_up="true"
  fi

  if [[ -z "$health_url" ]] || curl -fsS "$health_url" >/dev/null 2>&1; then
    target_healthy="true"
  fi

  if [[ -n "${companion_service:-}" ]]; then
    companion_up="false"
    companion_healthy="false"
    if docker compose ps "$companion_service" 2>/dev/null | tail -n +2 | grep -qE "(Up|running|healthy)"; then
      companion_up="true"
    fi
    if [[ -z "${companion_health_url:-}" ]] || curl -fsS "$companion_health_url" >/dev/null 2>&1; then
      companion_healthy="true"
    fi
  fi

  if [[ "$target_up" == "true" && "$target_healthy" == "true" && "$companion_up" == "true" && "$companion_healthy" == "true" ]]; then
    if [[ -n "${companion_service:-}" ]]; then
      echo "[banana] profile '$profile' is ready (react + fastify services + health checks)"
    elif [[ -z "$health_url" ]]; then
      echo "[banana] profile '$profile' is ready (service status)"
    else
      echo "[banana] profile '$profile' is ready (service + health check)"
    fi
    exit 0
  fi

  sleep "$interval_sec"
done

echo "[banana] readiness timeout for profile '$profile' service '$target_service'" >&2
if [[ -n "$health_url" ]]; then
  echo "[banana] last health check target: $health_url" >&2
fi
if [[ -n "${companion_health_url:-}" ]]; then
  echo "[banana] last companion health target: ${companion_health_url}" >&2
fi
exit 1
