#!/usr/bin/env bash
# Spec 012 — bring up the apps profile.
set -euo pipefail
cd "$(dirname "$0")/.."
exec docker compose --profile apps up -d "$@"
