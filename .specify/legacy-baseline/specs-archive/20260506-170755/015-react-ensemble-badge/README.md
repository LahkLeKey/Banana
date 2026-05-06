# 015 React `BananaBadge` Ensemble Variant -- Execution Tracker

**Status**: COMPLETE -- 20/20 tasks done.

## Result summary

- New shared type `EnsembleVerdict` in
  [src/typescript/shared/ui/src/types.ts](../../../src/typescript/shared/ui/src/types.ts).
- `BananaBadge` now supports `variant="ensemble"` with escalation pill and
  opt-in attention chip:
  [BananaBadge.tsx](../../../src/typescript/shared/ui/src/components/BananaBadge.tsx).
- New typed helper `fetchEnsembleVerdict` in
  [api.ts](../../../src/typescript/react/src/lib/api.ts).
- App wired through `VITE_BANANA_SHOW_ATTENTION` flag in
  [App.tsx](../../../src/typescript/react/src/App.tsx).
- Tests: 8 new component tests in
  [BananaBadge.test.tsx](../../../src/typescript/react/src/BananaBadge.test.tsx)
  + 1 new contract snapshot in
  [api.test.ts](../../../src/typescript/react/src/lib/api.test.ts).
  Total `bun test` lane: 15/15 pass.

## Adaptations from plan

- Component tests live in the react package (not shared/ui) because
  shared/ui has no test runner deps and slice 015 forbids new Bun
  packages. `react-dom/server.renderToStaticMarkup` is used for snapshots.

## Source

Built from
[../013-ml-brain-composition-spike/analysis/followup-B-react-ensemble-badge.md](../013-ml-brain-composition-spike/analysis/followup-B-react-ensemble-badge.md).

## Trigger to start

Slice 014 merged AND `EnsembleVerdictResult` JSON snapshot file exists in
`tests/integration/Banana.IntegrationTests/Pipeline/__snapshots__/` (or
equivalent location chosen by 014).

## Phases (mapped to tasks.md)

1. Setup (T001-T003)
2. Foundational types (T004-T005)
3. US1 Badge variant (T006-T009)
4. US2 Escalation pill (T010-T011)
5. US3 Optional attention chip (T012-T014)
6. API helper (T015-T017)
7. Validation + close-out (T018-T020)
