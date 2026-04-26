# 018 SPIKE -- Slice 019 readiness packet

Source: T006. Bounded scope for the implementation slice that picks up
this SPIKE's recommendation.

## Slice 019 name

`019-validation-lane-parity-lint`

## In-scope files (NEW)

- `scripts/validate-spec-tasks-parity.py` -- parser + checker.
- `scripts/validate-spec-tasks-parity.sh` -- thin wrapper for local
  invocation.
- `.github/workflows/validate-spec-tasks-parity.yml` -- CI job; runs
  on PRs that touch `.specify/specs/**`.
- `tests/scripts/test_validate_spec_tasks_parity.py` -- pytest unit
  tests for the parser (positive: clean slice 014; negative: slice 017
  pre-fix).
- `.specify/specs/017-ensemble-verdict-embedding-passthrough/tasks.md`
  -- backfill the missing `bash scripts/check-dotnet-coverage-threshold.sh`
  task (this is slice 019 PR-1's first acceptance test).
- `.specify/specs/010-...`, `011-...`, `012-...` tasks.md -- backfill
  coverage tasks OR remove the coverage threshold from spec.md (author
  decision per slice; tracked in slice 019).

## In-scope files (READ ONLY -- audit input)

- `.specify/specs/010-*` through `.specify/specs/017-*` `spec.md` and
  `tasks.md`.

## Validation commands

```
python scripts/validate-spec-tasks-parity.py .specify/specs/017-ensemble-verdict-embedding-passthrough
python -m pytest tests/scripts/test_validate_spec_tasks_parity.py
# CI job runs the lint across all slices touched by the PR.
```

## Acceptance bar

- A1: Lint script exits non-zero for slice 017 pre-backfill, zero
  post-backfill.
- A2: Lint script exits zero for slices 014, 015 unchanged.
- A3: Slices 010, 011, 012 either pass the lint (coverage tasks
  added) or have the coverage threshold removed from spec.md with a
  recorded rationale.
- A4: CI workflow blocks merge on lint failure.
- A5: Pytest covers parser edge cases: fenced code blocks, inline
  commands, multi-line commands with `\`, comments, headerless specs.

## Estimated task count

12-14 tasks. Phases:

1. Setup (1 task) -- repoint `.specify/feature.json`.
2. Parser + script (3 tasks) -- write parser, write checker, write
   pytest.
3. Backfill 017 (1 task) -- add the coverage threshold task.
4. Backfill / triage 010, 011, 012 (3 tasks) -- one per slice.
5. CI workflow (2 tasks) -- yml + smoke test.
6. Validation + close-out (2 tasks) -- run lint across all slices,
   close-out.

## Out of scope (deferred)

See [deferred-registry.md](./deferred-registry.md).
