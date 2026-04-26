# Tasks: React `BananaBadge` Ensemble Variant

**Branch**: `015-react-ensemble-badge` | **Created**: 2026-04-26

## Phase 1 -- Setup

- [ ] T001 Update `.specify/feature.json` -> `015-react-ensemble-badge`.
- [ ] T002 Author `README.md` execution tracker.
- [ ] T003 Author `analysis/in-scope-files.md` (link to SPIKE packet B).

## Phase 2 -- Foundational types

- [ ] T004 Add `EnsembleVerdict` type to
  `src/typescript/shared/ui/src/types.ts`.
- [ ] T005 Add a Zod (or hand-written) shape assertion test that mirrors
  the 014 `EnsembleVerdictResult` JSON snapshot.

## Phase 3 -- US1 Badge variant

- [ ] T006 Extend `src/typescript/shared/ui/src/components/BananaBadge.tsx`
  with `variant="ensemble"` prop.
- [ ] T007 Component test: clearly-banana anchor renders label + magnitude.
- [ ] T008 Component test: clearly-not-banana anchor renders label + magnitude.
- [ ] T009 Component test: ambiguous (escalated) anchors render label +
  magnitude + escalation pill.

## Phase 4 -- US2 Escalation pill

- [ ] T010 Render escalation pill when `did_escalate=true`; hide otherwise.
- [ ] T011 Component test: pill present iff `did_escalate=true`.

## Phase 5 -- US3 Optional attention chip

- [ ] T012 Add `VITE_BANANA_SHOW_ATTENTION` flag read in
  `src/typescript/react/src/App.tsx`.
- [ ] T013 Render attention chip when flag = `1`; default-off when absent.
- [ ] T014 Test: with flag absent, ensemble badge UI is byte-identical
  to a baseline snapshot taken with the flag absent.

## Phase 6 -- API helper

- [ ] T015 Add `fetchEnsembleVerdict(text)` helper in
  `src/typescript/react/src/lib/api.ts`.
- [ ] T016 Helper test snapshots the ensemble payload shape.
- [ ] T017 Wire `App.tsx` to call the helper and pass the result to the
  ensemble badge.

## Phase 7 -- Validation + close-out

- [ ] T018 `cd src/typescript/shared/ui && bun test`.
- [ ] T019 `cd src/typescript/react && bun test`.
- [ ] T020 Close `tasks.md`; update `README.md` with final status.

## Out of scope

- Native or managed changes.
- Electron preload bridge.
- New Bun packages.
