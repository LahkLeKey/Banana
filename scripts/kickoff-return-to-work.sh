#!/usr/bin/env bash
set -euo pipefail

AUTO_FIX=false
STATUS_ONLY=false
for arg in "$@"; do
  if [[ "$arg" == "--auto-fix" ]]; then
    AUTO_FIX=true
  elif [[ "$arg" == "--status-only" ]]; then
    STATUS_ONLY=true
  fi
done

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

echo "[kickoff] Return-to-work check starting"

if [[ "$STATUS_ONLY" == true ]]; then
  GIT_DIRTY=false
  if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    GIT_BRANCH="$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
    if [[ -n "$(git status --porcelain 2>/dev/null)" ]]; then
      GIT_DIRTY=true
      echo "[kickoff] GIT: DIRTY (branch: $GIT_BRANCH)"
      echo "[kickoff] GIT: Local changes detected; results may differ from CI"
    else
      echo "[kickoff] GIT: CLEAN (branch: $GIT_BRANCH)"
    fi
  else
    echo "[kickoff] GIT: status unavailable (not a git worktree)"
  fi
fi

if ! command -v bun >/dev/null 2>&1; then
  echo "[kickoff] PRECHECK FAIL: bun is not installed or not on PATH"
  echo "[kickoff] Fix: install bun, then re-run this command"
  if [[ "$STATUS_ONLY" == true ]]; then
    echo "[kickoff] STATUS: NOT READY"
  fi
  exit 2
fi

if ! bun --cwd src/typescript/react -e 'import "react/jsx-dev-runtime"; console.log("ok");' >/dev/null 2>&1; then
  echo "[kickoff] PRECHECK FAIL: TypeScript workspace dependencies are missing or incomplete"
  if [[ "$STATUS_ONLY" == true ]]; then
    echo "[kickoff] STATUS: NOT READY"
    echo "[kickoff] SUMMARY: ATTENTION NEEDED"
    echo "[kickoff] Fix: run bun install --cwd src/typescript"
    exit 2
  fi
  if [[ "$AUTO_FIX" == true ]]; then
    echo "[kickoff] AUTO-FIX: running bun install --cwd src/typescript"
    bun install --cwd src/typescript
    echo "[kickoff] AUTO-FIX: dependency install complete"
  else
    echo "[kickoff] Fix: run bun install --cwd src/typescript"
    echo "[kickoff] Or run auto-fix mode: npm run kickoff:return:auto-fix"
    echo "[kickoff] Then run: npm run kickoff:return"
    exit 2
  fi
fi

if [[ "$STATUS_ONLY" == true ]]; then
  echo "[kickoff] STATUS: READY"
  if [[ "$GIT_DIRTY" == false ]]; then
    echo "[kickoff] SUMMARY: ALL GREEN"
  else
    echo "[kickoff] SUMMARY: READY WITH LOCAL CHANGES"
  fi
  echo "[kickoff] Next command: npm run kickoff:return"
  exit 0
fi

echo "[kickoff] Running API smoke check..."

API_SMOKE_CMD='import { registerHealthRoutes } from "./src/typescript/api/src/routes/health.ts"; console.log(typeof registerHealthRoutes);'
UI_SMOKE_CMD='import "./src/typescript/shared/ui/src/index.ts"; console.log("ui-contract-ok");'

set +e
API_OUTPUT="$(bun -e "$API_SMOKE_CMD" 2>&1)"
API_EXIT=$?
set -e

if [[ $API_EXIT -eq 0 ]]; then
  echo "[kickoff] PASS: API smoke check succeeded"
  echo "[kickoff] Running UI smoke check..."

  set +e
  UI_OUTPUT="$(bun -e "$UI_SMOKE_CMD" 2>&1)"
  UI_EXIT=$?
  set -e

  if [[ $UI_EXIT -eq 0 ]]; then
    echo "[kickoff] PASS: UI smoke check succeeded"
    echo "[kickoff] Next steps:"
    echo "  1) Continue with native tasks if needed"
    echo "  2) Follow docs/agents/return-to-work-checklist.md"
    exit 0
  fi

  echo "[kickoff] FAIL: UI smoke check failed"
  echo "[kickoff] Output:"
  echo "$UI_OUTPUT"

  echo "[kickoff] Next steps:"
  echo "  1) Stop and do not continue other tasks"
  echo "  2) Capture: task name, first error line, last error line, what changed"
  echo "  3) Open or update a GitHub issue"
  echo "  4) Apply intake label (usually needs-triage)"
  echo "  5) See docs/agents/return-to-work-checklist.md"
  exit $UI_EXIT
fi

echo "[kickoff] FAIL: API smoke check failed"
echo "[kickoff] Output:"
echo "$API_OUTPUT"

echo "[kickoff] Next steps:"
echo "  1) Stop and do not continue other tasks"
echo "  2) Capture: task name, first error line, last error line, what changed"
echo "  3) Open or update a GitHub issue"
echo "  4) Apply intake label (usually needs-triage)"
echo "  5) See docs/agents/return-to-work-checklist.md"

exit $API_EXIT
