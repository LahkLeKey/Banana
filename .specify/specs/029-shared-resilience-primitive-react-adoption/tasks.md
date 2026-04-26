# Tasks: Shared Resilience Primitive + React Adoption

**Branch**: `029-shared-resilience-primitive-react-adoption` | **Created**: 2026-04-26
**Status**: GATED. Foundational for slices 030 + 031.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `029-shared-resilience-primitive-react-adoption`.
- [ ] T002 Author `README.md` execution tracker.

## Phase 2 -- Shared package skeleton

- [ ] T003 Create `src/typescript/shared/resilience/package.json`
  (name `@banana/resilience`).
- [ ] T004 Create `src/typescript/shared/resilience/tsconfig.json`.
- [ ] T005 Author `src/typescript/shared/resilience/src/types.ts`
  per the storage contract.

## Phase 3 -- Primitive impl

- [ ] T006 Author `src/typescript/shared/resilience/src/retry.ts`.
- [ ] T007 Author `src/typescript/shared/resilience/src/queue.ts`
  (RequestQueue with dedupe-by-key + retry).
- [ ] T008 Author `src/typescript/shared/resilience/src/history.ts`
  (VerdictHistory).
- [ ] T009 Author `src/typescript/shared/resilience/src/adapters/indexeddb.ts`.
- [ ] T010 Author `src/typescript/shared/resilience/src/index.ts`
  re-exports.

## Phase 4 -- Tests

- [ ] T011 Add `src/typescript/shared/resilience/queue.test.ts`
  covering happy path + retry exhaustion + dedupe.
- [ ] T012 Add `src/typescript/shared/resilience/history.test.ts`.

## Phase 5 -- React adoption

- [ ] T013 Add `src/typescript/react/src/lib/resilience-bootstrap.ts`.
- [ ] T014 In `src/typescript/react/src/App.tsx`, wire queue +
  history; add online-listener drain trigger; preserve draft on
  failure; surface last-N verdicts.
- [ ] T015 Add `@banana/resilience` to `src/typescript/react/package.json`.

## Phase 6 -- Close-out

- [ ] T016 Run validation lane
  (`bun run --cwd src/typescript/shared/resilience tsc --noEmit`,
  `bun run --cwd src/typescript/shared/resilience test`,
  `bun run --cwd src/typescript/react tsc --noEmit`,
  `bun run --cwd src/typescript/react test`).
- [ ] T017 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Electron / RN adoption (slices 030 / 031).
- Cross-device sync (D-022-01).
- Encryption at rest (D-022-03).
