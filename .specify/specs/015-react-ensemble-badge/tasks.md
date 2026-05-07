# Tasks: React `BananaBadge` Ensemble Variant

**Branch**: `015-react-ensemble-badge` | **Created**: 2026-04-26

## Phase 1 -- Setup

- [x] T001 Update `.specify/feature.json` -> `015-react-ensemble-badge`.
- [x] T002 Author `README.md` execution tracker.
- [x] T003 Author `analysis/in-scope-files.md` (link to SPIKE packet B).

## Phase 2 -- Foundational types

- [x] T004 Add `EnsembleVerdict` type to
  `src/typescript/shared/ui/src/types.ts`.
- [x] T005 Add a Zod (or hand-written) shape assertion test that mirrors
  the 014 `EnsembleVerdictResult` JSON snapshot.

## Phase 3 -- US1 Badge variant

- [x] T006 Extend `src/typescript/shared/ui/src/components/BananaBadge.tsx`
  with `variant="ensemble"` prop.
- [x] T007 Component test: clearly-banana anchor renders label + magnitude.
- [x] T008 Component test: clearly-not-banana anchor renders label + magnitude.
- [x] T009 Component test: ambiguous (escalated) anchors render label +
  magnitude + escalation pill.

## Phase 4 -- US2 Escalation pill

- [x] T010 Render escalation pill when `did_escalate=true`; hide otherwise.
- [x] T011 Component test: pill present iff `did_escalate=true`.

## Phase 5 -- US3 Optional attention chip

- [x] T012 Add `VITE_BANANA_SHOW_ATTENTION` flag read in
  `src/typescript/react/src/App.tsx`.
- [x] T013 Render attention chip when flag = `1`; default-off when absent.
- [x] T014 Test: with flag absent, ensemble badge UI is byte-identical
  to a baseline snapshot taken with the flag absent.

## Phase 6 -- API helper

- [x] T015 Add `fetchEnsembleVerdict(text)` helper in
  `src/typescript/react/src/lib/api.ts`.
- [x] T016 Helper test snapshots the ensemble payload shape.
- [x] T017 Wire `App.tsx` to call the helper and pass the result to the
  ensemble badge.

## Phase 7 -- Validation + close-out

- [x] T018 `cd src/typescript/shared/ui && bun test`.
- [x] T019 `cd src/typescript/react && bun test`.
- [x] T020 Close `tasks.md`; update `README.md` with final status.

## Out of scope

- Native or managed changes.
- Electron preload bridge.
- New Bun packages.
