## Problem Statement

## In Scope

- Feature implementation and integration
- Testing and validation of new behavior
- Documentation of feature usage and design decisions

Feature Specification: Validation Lane Enforcement SPIKE aims to improve system capability and user experience by implementing the feature described in the specification.

# Feature Specification: Validation Lane Enforcement SPIKE

**Feature Branch**: `[018-validation-lane-enforcement-spike]`
**Created**: 2026-04-26
**Status**: Draft -- ready to investigate
**Type**: SPIKE (investigate -> plan a single bounded follow-up spec)
**Triggered by**: Slice 017 close-out gap. `spec.md`'s Validation lane
listed `scripts/check-dotnet-coverage-threshold.sh` as part of the
slice's success bar, but `tasks.md` only required `dotnet test`. The
coverage gate was silently skipped at close-out and only surfaced in a
post-hoc reviewer comment. This is a recurring failure mode across
prior slices and warrants a deliberate fix.

## Problem

Spec Kit slices encode their validation bar in two places:

1. `spec.md` Validation lane / Success criteria (authoritative intent).
2. `tasks.md` Validation phase tasks (executable checklist).

When the two drift, close-out follows `tasks.md` and the spec-level
intent is lost. Because tasks are bulk-marked `[x]` with a one-line
regex script, no human or tool re-reads `spec.md` at close to verify
parity. The result: optional-but-required validation steps (coverage
gates, snapshot locks, ABI assertions, contract diffs) get skipped
without a paper trail.

## Investigation goals

- **G1**: Quantify the drift. Audit slices 010-017 for validation
  lane items in `spec.md` that have no corresponding task in
  `tasks.md`. Output: gap matrix (slice x missing-validator).
- **G2**: Classify the failure modes. Identify whether the drift is
  caused by (a) `/speckit.tasks` template omissions, (b) author
  shorthand, (c) cross-slice copy-paste decay, or (d) absence of a
  parity check.
- **G3**: Pick exactly one enforcement strategy from the candidate set:
  - **A**: Lint script `scripts/validate-spec-tasks-parity.py` that
    fails CI when `spec.md` Validation lane items are not referenced
    in `tasks.md`.
  - **B**: Mandatory close-out task template
    (`T999 Run spec.md Validation lane verbatim`) injected into
    `/speckit.tasks` output.
  - **C**: Hook in `.specify/scripts/bash/finalize-feature.sh` that
    re-parses `spec.md` and refuses to mark complete until each
    validation command has been logged.
- **G4**: Produce a readiness packet for the chosen strategy that the
  follow-up implementation slice can pick up without re-litigation.

## Out of Scope

- Implementing the chosen enforcement strategy (that is the follow-up
  spec, slice 019).
- Changing the slice 017 result (it is closed; the coverage gate can
  be backfilled in 019's first PR).
- Reworking unrelated `.specify/` workflows (constitution, plan,
  analyze).

## Deliverables

- `analysis/drift-audit.md` -- gap matrix for slices 010-017.
- `analysis/failure-modes.md` -- classification of root causes.
- `analysis/enforcement-options.md` -- pros/cons of A/B/C with
  recommendation.
- `analysis/followup-readiness-packet.md` -- bounded scope for slice
  019 (one chosen strategy, in-scope files, validation commands,
  estimated task count).
- `analysis/deferred-registry.md` -- any items pushed past 019.

## Success Criteria

- Audit covers every slice from 010 forward.
- Exactly one enforcement strategy recommended with explicit rationale
  tied to audit evidence.
- Follow-up readiness packet is small enough that slice 019 can be
  scoped to <=15 tasks.
- No code or workflow changes land in this SPIKE.

## Validation lane

```
# SPIKE-only -- no build/test required.
# Verification is human review of the four deliverables above.
ls .specify/specs/018-validation-lane-enforcement-spike/analysis/
```

## Downstream

This SPIKE unlocks slice **019 Validation Lane Enforcement
Implementation** with a pre-decided strategy and bounded scope. The
slice 017 coverage gate backfill rides on slice 019's first PR.

## In-scope files

- `.specify/specs/010-*` through `.specify/specs/017-*` (READ ONLY --
  audit input).
- `.specify/specs/018-validation-lane-enforcement-spike/analysis/**`
  (NEW -- SPIKE outputs).
- `.specify/feature.json` (repoint to 018 during execution).
