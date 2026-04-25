#!/usr/bin/env bash
# Local Electron desktop fallback (spec 010 channel "Local fallback").
# Containerized launchers live under scripts/.
set -euo pipefail
cd "$(dirname "$0")"
exec npm start
