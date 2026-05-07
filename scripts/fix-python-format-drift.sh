#!/usr/bin/env bash
# fix-python-format-drift.sh
# Run ruff format + ruff lint --fix on all Python source directories.
# Use this before push or after merging formatting tool upgrades to avoid
# pre-commit gate failures caused by formatter drift.
#
# Usage:
#   bash scripts/fix-python-format-drift.sh          # apply fixes
#   bash scripts/fix-python-format-drift.sh --check  # check only (exit 1 if drift)
#
# Spec: .specify/specs/110-precommit-ruff-format-stability

set -euo pipefail

RUFF="${RUFF:-ruff}"
MODE="${1:-}"

DIRS=(scripts/ tests/)

if [[ "$MODE" == "--check" ]]; then
  echo "[fix-python-format-drift] checking for drift (no writes)..."
  "$RUFF" format "${DIRS[@]}" --check
  "$RUFF" check "${DIRS[@]}"
  echo "[fix-python-format-drift] no drift found."
else
  echo "[fix-python-format-drift] applying ruff format + lint fixes..."
  "$RUFF" format "${DIRS[@]}"
  "$RUFF" check "${DIRS[@]}" --fix
  echo "[fix-python-format-drift] done."
fi
