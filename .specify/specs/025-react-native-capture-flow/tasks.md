# Tasks: React Native Capture Flow

**Branch**: `025-react-native-capture-flow` | **Created**: 2026-04-26
**Status**: GATED.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `025-react-native-capture-flow`.
- [ ] T002 Author `README.md` execution tracker.

## Phase 2 -- API lib

- [ ] T003 Add `src/typescript/react-native/lib/api.ts` mirroring
  React `lib/api.ts` (verdict + ensemble + embedding fetch helpers).

## Phase 3 -- Capture + verdict screens

- [ ] T004 Add `src/typescript/react-native/screens/CaptureScreen.tsx`
  with text-paste entry and canonical empty-state copy.
- [ ] T005 Add `src/typescript/react-native/screens/VerdictScreen.tsx`
  rendering canonical verdict copy + escalation cue.
- [ ] T006 Replace stub `BananaBadge count={0}` + hard-coded
  `RipenessLabel` in `src/typescript/react-native/index.tsx` with
  navigation to the new screens.

## Phase 4 -- Native escalation panel

- [ ] T007 Add `src/typescript/shared/ui/src/native/EscalationPanel.tsx`
  mirroring slice 023's web variant.
- [ ] T008 Export from `src/typescript/shared/ui/src/native/index.ts`.

## Phase 5 -- Share-sheet ingest

- [ ] T009 Add `src/typescript/react-native/share-handler.ts` that
  registers a share-sheet listener and routes payloads to
  `CaptureScreen`.
- [ ] T010 Update `src/typescript/react-native/app.json` with
  Android intent-filter and iOS share-extension declarations.

## Phase 6 -- Close-out

- [ ] T011 Run validation lane
  (`bun run --cwd src/typescript/shared/ui tsc --noEmit`,
  `bun run --cwd src/typescript/react-native tsc --noEmit`).
- [ ] T012 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Camera (D-020-02).
- Tokens (SPIKE 021).
- Offline / persistence (SPIKE 022 / slice 031).
