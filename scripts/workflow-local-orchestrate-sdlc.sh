#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

export BANANA_LOCAL_DRY_RUN="${BANANA_LOCAL_DRY_RUN:-true}"
export BANANA_WIKI_DRY_RUN="${BANANA_WIKI_DRY_RUN:-true}"
export BANANA_WIKI_PUSH="${BANANA_WIKI_PUSH:-false}"
export BANANA_SDLC_ENABLE_WIKI_SYNC="${BANANA_SDLC_ENABLE_WIKI_SYNC:-true}"
export BANANA_SDLC_REQUIRE_REAL_UPDATES="${BANANA_SDLC_REQUIRE_REAL_UPDATES:-true}"
export BANANA_SDLC_SKIP_NON_REAL_UPDATES="${BANANA_SDLC_SKIP_NON_REAL_UPDATES:-true}"
export BANANA_REAL_UPDATE_PATH_PATTERNS="${BANANA_REAL_UPDATE_PATH_PATTERNS:-src/**,tests/**,scripts/**,.github/workflows/**,docker/**,docker-compose.yml,CMakeLists.txt,Directory.Build.props}"
export BANANA_WIKI_DIR="${BANANA_WIKI_DIR:-artifacts/wiki-sync/local-wiki-dry-run}"

bash scripts/workflow-orchestrate-sdlc.sh
