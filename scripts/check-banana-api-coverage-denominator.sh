#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
python scripts/sync-banana-api-coverage-denominator.py --check
