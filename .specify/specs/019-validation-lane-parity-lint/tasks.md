# Tasks: Validation Lane Parity Lint

**Branch**: `019-validation-lane-parity-lint` | **Created**: 2026-04-26

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` -> `019-validation-lane-parity-lint`.
- [x] T002 Author `analysis/in-scope-files.md`.

## Phase 2 -- Parser + checker

- [x] T003 Implement `scripts/validate-spec-tasks-parity.py` (parser
  for `spec.md` Validation lane + Success criteria; checker against
  `tasks.md`; per-validator diagnostic on drift; exit non-zero on
  drift).
- [x] T004 Implement `scripts/validate-spec-tasks-parity.sh` thin
  wrapper supporting `<feature-dir>` and `--all`.
- [x] T005 Pytest in `tests/scripts/test_validate_spec_tasks_parity.py`
  covering: clean slice (014), drift slice (synthetic), edge cases
  (fenced blocks, inline commands, comments).

## Phase 3 -- Backfill

- [x] T006 Backfill `017` `tasks.md`: add a task for
  `bash scripts/check-dotnet-coverage-threshold.sh` and add the
  `--collect:"XPlat Code Coverage"` flag to the dotnet test task.
- [x] T007 Backfill `010` `tasks.md`: add a task asserting regression
  path coverage `>= 90%` line coverage OR remove that line from
  `spec.md` Success Criteria with rationale.
- [x] T008 Backfill `011` `tasks.md`: same treatment for binary path
  coverage `>= 90%`.
- [x] T009 Backfill `012` `tasks.md`: same treatment for transformer
  path coverage `>= 85%`.

## Phase 4 -- CI

- [x] T010 Add `.github/workflows/validate-spec-tasks-parity.yml`
  triggered on PRs touching `.specify/specs/**`. Runs the lint
  across changed feature dirs; fails on non-zero exit.
- [x] T011 Smoke-test the workflow locally via
  `bash scripts/validate-spec-tasks-parity.sh --all`.

## Phase 5 -- Validation + close-out

- [x] T012 `python -m pytest tests/scripts/test_validate_spec_tasks_parity.py`
  green.
- [x] T013 `bash scripts/validate-spec-tasks-parity.sh --all` -- exit 0
  across slices 010..017.
- [x] T014 Mark all 019 tasks `[x]`; update `README.md` to COMPLETE.

## Out of scope

- Finalize hook (D-001 in 018).
- `validation-evidence.md` convention (D-002 in 018).
- Native / ASP.NET / React / compose changes.
