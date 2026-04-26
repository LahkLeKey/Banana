#!/usr/bin/env bash
# Spec 011 — Ubuntu-side mobile emulator runtime (Android via WSLg).
# Profile-gated: requires --profile apps + --profile android-emulator.
# Exit 42 = preflight failure.
set -euo pipefail

require() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "[banana] missing prerequisite: $1" >&2
    exit 42
  }
}

require docker
[[ -e /dev/kvm ]] || {
  echo "[banana] /dev/kvm missing — Android emulator requires KVM in WSL2" >&2
  exit 42
}

cd "$(dirname "$0")/.."
exec docker compose --profile apps --profile android-emulator up android-emulator
