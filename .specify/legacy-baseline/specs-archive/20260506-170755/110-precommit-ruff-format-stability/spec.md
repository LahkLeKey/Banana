# Feature Specification: Pre-commit Ruff Format Stability

**Feature Branch**: `110-precommit-ruff-format-stability`
**Created**: 2026-05-02
**Status**: Follow-up stabilization scaffold
**Wave**: stabilization
**Domain**: workflow / tooling
**Depends on**: #106, #107

## Problem Statement

Recent `main` pre-commit workflow failures are caused by `ruff-format` mutating tracked Python files during CI. The lane fails with "files were modified by this hook," indicating formatting drift is reaching CI instead of being normalized before merge.

## In Scope *(mandatory)*

- Normalize current Python formatting drift that triggers `ruff-format` rewrites in CI.
- Add guardrails so formatter drift is caught earlier in contributor and automation paths.
- Ensure generated/automation-authored Python helper scripts are pre-commit compliant before push.
- Preserve existing pre-commit security and style hooks.

## Out of Scope *(mandatory)*

- Removing `ruff-format` from pre-commit.
- Replacing the repo pre-commit framework.

## Success Criteria

- pre-commit workflow passes on `main` without hook-modified-file failures.
- Automation-generated Python files are consistently pre-formatted before merge.
- Contributors have deterministic local command path to reproduce and fix formatter drift.

## Notes for the planner

- Keep the fix bounded to formatting stability, not broad lint policy changes.
- Ensure CI failure messages remain actionable for local reproduction.
