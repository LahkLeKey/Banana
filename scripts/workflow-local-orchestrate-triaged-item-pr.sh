#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

export BANANA_TRIAGE_ID="${BANANA_TRIAGE_ID:-local-safe-noop}"
export BANANA_TRIAGE_CHANGE_COMMAND="${BANANA_TRIAGE_CHANGE_COMMAND:-:}"
export BANANA_BASE_BRANCH="${BANANA_BASE_BRANCH:-main}"
export BANANA_BRANCH_PREFIX="${BANANA_BRANCH_PREFIX:-triage}"
export BANANA_COMMIT_MESSAGE="${BANANA_COMMIT_MESSAGE:-chore(triage): automated triaged change}"
export BANANA_PR_TITLE="${BANANA_PR_TITLE:-}"
export BANANA_PR_BODY="${BANANA_PR_BODY:-}"
export BANANA_DRAFT_PR="${BANANA_DRAFT_PR:-true}"
export BANANA_PR_LABELS="${BANANA_PR_LABELS:-automation,triaged-item,requires-human-approval}"
export BANANA_PR_REVIEWERS="${BANANA_PR_REVIEWERS:-}"
export BANANA_LOCAL_DRY_RUN="${BANANA_LOCAL_DRY_RUN:-true}"

bash scripts/workflow-orchestrate-triaged-item-pr.sh
