#!/usr/bin/env bash
# Spec 014 — tests profile.
set -euo pipefail
cd "$(dirname "$0")/.."
exec docker compose --profile tests up --abort-on-container-exit
