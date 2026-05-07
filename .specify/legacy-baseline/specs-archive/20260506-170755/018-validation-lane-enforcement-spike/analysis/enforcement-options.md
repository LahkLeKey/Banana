# 018 SPIKE -- Enforcement options

Source: T004-T005. Three candidate strategies scored against the
drift evidence in [drift-audit.md](./drift-audit.md) and the failure
modes in [failure-modes.md](./failure-modes.md).

## Option A -- Lint script (`scripts/validate-spec-tasks-parity.py`)

A standalone Python script that parses `spec.md` Validation lane and
Success criteria for executable commands (anything in fenced code
blocks plus inline `bash`/`cmake`/`ctest`/`dotnet`/`bun` commands) and
verifies each command (or a recognizable substring) appears in
`tasks.md`. Wired into `.github/workflows/` and runnable locally via
`bash scripts/validate-spec-tasks-parity.sh <feature-dir>`.

| Dimension              | Score                                          |
| ---------------------- | ---------------------------------------------- |
| Enforcement strength   | HIGH -- CI-blocking, deterministic             |
| Author friction        | LOW -- runs locally in <1s; clear error output |
| CI cost                | LOW -- pure-Python, no build dependency        |
| Retro-fit cost         | MEDIUM -- 010/011/012 will fail until backfilled |
| Catches FM-A           | YES                                            |
| Catches FM-B           | YES                                            |

## Option B -- Mandatory close-out task template

Modify the `/speckit.tasks` skill output to always emit a final task
`T999 Run spec.md Validation lane verbatim and capture evidence in analysis/validation-evidence.md`.

| Dimension              | Score                                          |
| ---------------------- | ---------------------------------------------- |
| Enforcement strength   | LOW -- still relies on author honesty at `[x]` |
| Author friction        | LOW                                            |
| CI cost                | NONE                                           |
| Retro-fit cost         | LOW                                            |
| Catches FM-A           | PARTIAL (only on new slices)                   |
| Catches FM-B           | NO -- same bulk-`[x]` failure mode applies     |

## Option C -- Finalize-feature hook

Add a hook in `.specify/scripts/bash/finalize-feature.sh` that
re-parses `spec.md` and refuses to mark complete until each validation
command has been executed and logged into
`analysis/validation-evidence.md`.

| Dimension              | Score                                          |
| ---------------------- | ---------------------------------------------- |
| Enforcement strength   | HIGH (only if hook is mandatory)               |
| Author friction        | MEDIUM -- requires running the hook locally    |
| CI cost                | LOW                                            |
| Retro-fit cost         | HIGH -- requires reshaping the close-out flow  |
| Catches FM-A           | YES                                            |
| Catches FM-B           | YES                                            |

## Recommendation

**Option A -- lint script wired into CI.**

Tied to drift evidence:

- Slices 010, 011, 012 (HIGH gaps) would have failed CI immediately
  on PR open, forcing the author to either add coverage tasks or
  remove the coverage threshold from `spec.md`. Either resolution is
  acceptable; both eliminate silent drift.
- Slice 017 would have failed CI on the close-out PR, surfacing the
  missing `check-dotnet-coverage-threshold.sh` task before merge.
- Option B does not catch FM-B (the dominant cause for current
  slices) so it is rejected.
- Option C is functionally equivalent in strength to A but has higher
  retro-fit cost and depends on a local-only hook that authors can
  bypass. Rejected as redundant with A.

A is also the only option that produces machine-readable evidence
that can be linked from each slice's README ("parity check: PASS at
commit `<sha>`"), which doubles as a slice-level audit trail.
