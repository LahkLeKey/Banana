# Plan: ML Brain Composition & Productionization SPIKE

**Branch**: `013-ml-brain-composition-spike` | **Date**: 2026-04-26

## Objective

Convert the discovery output of `research.md` into bounded, selection-ready
readiness packets for the three recommended downstream slices.

## Constraints

- SPIKE is read-only across `src/native/**`, `src/c-sharp/**`, and
  `src/typescript/**`. Only `.specify/specs/013-ml-brain-composition-spike/**`
  files change.
- Native lane MUST stay at 7/7. SPIKE MUST NOT modify `tests/native/**`.
- All recommendations MUST cite anchors from
  `010-left-brain-regression-runnable`, `011-right-brain-binary-runnable`,
  or `012-full-brain-transformer-runnable`.

## Deliverables

- `spec.md` (done)
- `research.md` (done)
- `plan.md` (this file)
- `tasks.md` -- SPIKE-only task list
- `analysis/composition-strategy.md` -- expanded R-01 with worked anchor walks
- `analysis/consumer-mapping.md` -- expanded R-02 with file paths and
  controller/pipeline names
- `analysis/deferred-registry.md` -- refreshed registry per R-03
- `analysis/followup-A-aspnet-ensemble.md` -- readiness packet
- `analysis/followup-B-react-ensemble-badge.md` -- readiness packet
- `analysis/followup-C-quantized-embedding.md` -- readiness packet
  (deferred-with-trigger; packet exists but is gated by R-03 trigger)

## Readiness packet template

Each follow-up packet MUST contain:

1. **Objective** -- one sentence, outcome-focused.
2. **In-scope files** -- explicit list under one domain.
3. **Out-of-scope** -- explicit non-changes.
4. **Validation lane** -- exact commands.
5. **Contract risk** -- additive-ABI / JSON-shape impact, if any.
6. **Anchor references** -- citations to 010/011/012 evidence files.
7. **Owner agent** -- nearest helper agent per the agent decomposition skill.

## Phases

- Phase 1: Author `analysis/composition-strategy.md`, `consumer-mapping.md`,
  `deferred-registry.md` (one file each, expanding R-01..R-03).
- Phase 2: Draft the three follow-up packets (A, B, C).
- Phase 3: Cross-link packets back from `README.md`; mark SPIKE complete.

## Out of scope for this SPIKE

- Writing any C, C#, TS, or test code.
- Bumping `BANANA_WRAPPER_ABI_VERSION_*`.
- Editing `.specify/feature.json` (the active feature pointer remains on 012
  unless the user chooses to switch it).
