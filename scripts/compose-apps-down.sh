#!/usr/bin/env bash
# Spec 012 — tear down apps profile.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"
exec bash scripts/compose-run-profile.sh --profile apps --action down -- "$@"
