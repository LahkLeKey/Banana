# Feature Specification: Validation Lane Parity Lint

**Feature Branch**: `[019-validation-lane-parity-lint]`
**Created**: 2026-04-26
**Status**: Draft -- ready to execute
**SPIKE source**: [018-validation-lane-enforcement-spike](../018-validation-lane-enforcement-spike/spec.md)
**Readiness packet**: [018/analysis/followup-readiness-packet.md](../018-validation-lane-enforcement-spike/analysis/followup-readiness-packet.md)

## In Scope

- Add `scripts/validate-spec-tasks-parity.py` parser + checker.
- Add `scripts/validate-spec-tasks-parity.sh` thin wrapper.
- Add `tests/scripts/test_validate_spec_tasks_parity.py` pytest unit
  tests.
- Add `.github/workflows/validate-spec-tasks-parity.yml` CI job that
  runs on PRs touching `.specify/specs/**`.
- Backfill missing validators in `tasks.md` for slices 010, 011, 012,
  017 OR remove the unmet validator from `spec.md` with a recorded
  rationale (per-slice author decision).

## Out of Scope

- Changing `spec.md` Validation lane semantics (remains authoritative).
- Implementing option C (finalize hook) -- deferred (D-001 in 018).
- Adding a `validation-evidence.md` requirement -- deferred (D-002).
- Native, ASP.NET, React, or compose changes.

## Requirements

- **L-R01**: Parser MUST extract executable commands from `spec.md`
  Validation lane fenced blocks AND from inline backticked commands
  in Success Criteria sections.
- **L-R02**: Checker MUST consider a command "present" in `tasks.md`
  if a recognizable substring (longest token sequence >= 3 tokens)
  appears in any task line. False-positive prevention via token
  threshold; false-negative prevention by stripping flags before
  match.
- **L-R03**: Exit code 0 = pass; exit code non-zero = drift detected
  with per-validator diagnostic.
- **L-R04**: CI workflow MUST block merge on non-zero exit.
- **L-R05**: All currently-clean slices (014, 015) MUST continue to
  pass without modification.
- **L-R06**: Slice 017 MUST pass after backfill of the
  `check-dotnet-coverage-threshold.sh` task.

## Success Criteria

- Pytest green.
- Lint exits 0 across all slices 010..017 post-backfill.
- CI workflow demonstrably blocks a synthetic drift PR.

## Validation lane

```
python -m pytest tests/scripts/test_validate_spec_tasks_parity.py
python scripts/validate-spec-tasks-parity.py .specify/specs/017-ensemble-verdict-embedding-passthrough
bash scripts/validate-spec-tasks-parity.sh --all
```

## In-scope files

See [analysis/in-scope-files.md](./analysis/in-scope-files.md).
