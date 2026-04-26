# Tasks: React Native Persistence + Retry

**Branch**: `031-react-native-persistence-retry` | **Created**: 2026-04-26
**Status**: GATED. Hard prerequisite: slice 029.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `031-react-native-persistence-retry`.
- [ ] T002 Author `README.md` execution tracker.

## Phase 2 -- AsyncStorage adapter

- [ ] T003 Add `src/typescript/shared/resilience/src/adapters/async-storage.ts`.
- [ ] T004 Add adapter contract test reusing the shared suite from
  slice 029 (parametrized over storage backend).

## Phase 3 -- RN bootstrap + adoption

- [ ] T005 Add `src/typescript/react-native/lib/resilience-bootstrap.ts`.
- [ ] T006 Wire queue + history into
  `src/typescript/react-native/index.tsx`; preserve draft on failure.
- [ ] T007 Add `@banana/resilience`,
  `@react-native-async-storage/async-storage`,
  `@react-native-community/netinfo` to
  `src/typescript/react-native/package.json`.
- [ ] T008 Wire NetInfo online-listener drain trigger.

## Phase 4 -- History screen

- [ ] T009 Add `src/typescript/react-native/screens/HistoryScreen.tsx`
  surfacing last-N verdicts via `VerdictHistory.list()`.
- [ ] T010 Add navigation entry to history screen.

## Phase 5 -- Close-out

- [ ] T011 Run validation lane
  (`bun run --cwd src/typescript/shared/resilience tsc --noEmit`,
  `bun run --cwd src/typescript/shared/resilience test`,
  `bun run --cwd src/typescript/react-native tsc --noEmit`).
- [ ] T012 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Camera (D-020-02).
- iOS background-fetch tuning (D-022-04).
- Encryption at rest (D-022-03).
