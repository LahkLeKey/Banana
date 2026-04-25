#!/usr/bin/env bash
# Spec 012 — tear down apps profile.
set -euo pipefail
cd "$(dirname "$0")/.."
exec docker compose --profile apps down "$@"
