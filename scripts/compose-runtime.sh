#!/usr/bin/env bash
# Spec 012 — full runtime stack.
set -euo pipefail
cd "$(dirname "$0")/.."
exec docker compose --profile runtime up -d "$@"
