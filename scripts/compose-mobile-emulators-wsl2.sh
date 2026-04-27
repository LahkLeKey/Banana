#!/usr/bin/env bash
# Spec 011 — Ubuntu-side mobile emulator runtime (Android via WSLg).
# Profile-gated: requires --profile apps + --profile android-emulator.
# Exit 42 = preflight failure.
set -euo pipefail

require() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "[banana] missing prerequisite: $1" >&2
    echo "[banana] ensure Docker Desktop WSL integration is enabled for this distro" >&2
    exit 42
  }
}

require docker
if ! docker version >/dev/null 2>&1; then
  echo "[banana] docker server unavailable inside Ubuntu WSL2" >&2
  echo "[banana] enable Docker Desktop WSL integration and retry" >&2
  exit 42
fi

if [[ ! -S /var/run/docker.sock ]]; then
  echo "[banana] /var/run/docker.sock missing in Ubuntu WSL2 runtime" >&2
  echo "[banana] this is an environment contract failure; verify Docker Desktop WSL integration" >&2
  exit 42
fi

[[ -e /dev/kvm ]] || {
  echo "[banana] /dev/kvm missing — Android emulator requires KVM in WSL2" >&2
  echo "[banana] check WSL2 virtualization support and Android emulator prerequisites" >&2
  exit 42
}

cd "$(dirname "$0")/.."
bash scripts/compose-run-profile.sh --profile mobile --action up --service android-emulator
exec bash scripts/compose-profile-ready.sh --profile mobile --service android-emulator --timeout-sec "${BANANA_PROFILE_READY_TIMEOUT_SEC:-180}"
