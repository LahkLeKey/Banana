# Feature Specification: React `BananaBadge` Ensemble Variant

**Feature Branch**: `[015-react-ensemble-badge]`
**Created**: 2026-04-26
**Status**: Draft
**Depends on**: 014 (must be merged first; consumes `EnsembleVerdictResult` shape)
**SPIKE source**: [../013-ml-brain-composition-spike/analysis/followup-B-react-ensemble-badge.md](../013-ml-brain-composition-spike/analysis/followup-B-react-ensemble-badge.md)
## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Feature Specification: React `BananaBadge` Ensemble Variant aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Add `variant="ensemble"` to `BananaBadge` consuming the
  `EnsembleVerdictResult` shape from slice 014.
- Surface the calibration magnitude and the escalation flag in the badge.
- Add an opt-in attention chip behind `VITE_BANANA_SHOW_ATTENTION=1`.
- Add a typed React API helper for the new ensemble route.

## Out of Scope

- Native or ASP.NET changes.
- Electron preload bridge.
- New Bun packages.
- Tailwind config changes.

## User Scenarios

### US1 -- Ensemble verdict displayed (P1)
Render final label + calibration magnitude. Snapshot test for each of the
six anchor walks from the SPIKE composition strategy.

### US2 -- Escalation badge (P2)
When `did_escalate` is true, the badge shows an "escalated" pill.

### US3 -- Optional attention chip (P3)
Behind `VITE_BANANA_SHOW_ATTENTION=1`, show a small chip indicating which
brain produced the final verdict. Off by default; absent flag = UI
byte-identical to the non-ensemble variant.

## Requirements

- **R-R01**: TypeScript types MUST be derived from / aligned with the 014
  `EnsembleVerdictResult` JSON snapshot test.
- **R-R02**: Default-off attention chip MUST produce a UI byte-identical
  to the non-ensemble variant when the flag is absent.
- **R-R03**: API helper test MUST snapshot the ensemble shape so a 014
  contract change fails this lane.

## Validation lane

```
cd src/typescript/shared/ui && bun test
cd src/typescript/react && bun test
```

## In-scope files

See
[../013-ml-brain-composition-spike/analysis/followup-B-react-ensemble-badge.md](../013-ml-brain-composition-spike/analysis/followup-B-react-ensemble-badge.md).
