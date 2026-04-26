# Tasks: Shared UI Components v2

**Branch**: `027-shared-ui-components-v2` | **Created**: 2026-04-26
**Status**: GATED. Hard prerequisite: slice 026.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `027-shared-ui-components-v2`.
- [ ] T002 Author `README.md` execution tracker.

## Phase 2 -- Refactor existing web components to tokens

- [ ] T003 Refactor `src/typescript/shared/ui/src/components/BananaBadge.tsx`
  to consume tokens (preserve public props, byte-identical render).
- [ ] T004 Refactor `src/typescript/shared/ui/src/components/RipenessLabel.tsx`.
- [ ] T005 Refactor `src/typescript/shared/ui/src/components/ChatMessageBubble.tsx`.

## Phase 3 -- New web components

- [ ] T006 Add `src/typescript/shared/ui/src/components/EscalationPanel.tsx`.
- [ ] T007 Add `src/typescript/shared/ui/src/components/RetryButton.tsx`.
- [ ] T008 Add `src/typescript/shared/ui/src/components/ErrorText.tsx`.

## Phase 4 -- Refactor + add native components

- [ ] T009 Refactor `src/typescript/shared/ui/src/native/BananaBadge.tsx`
  to consume tokens AND add ensemble variant (closes gap).
- [ ] T010 Refactor `src/typescript/shared/ui/src/native/ChatMessageBubble.tsx`.
- [ ] T011 Add `src/typescript/shared/ui/src/native/RipenessLabel.tsx`
  (closes gap).
- [ ] T012 Add `src/typescript/shared/ui/src/native/EscalationPanel.tsx`.
- [ ] T013 Add `src/typescript/shared/ui/src/native/RetryButton.tsx`.
- [ ] T014 Add `src/typescript/shared/ui/src/native/ErrorText.tsx`.

## Phase 5 -- Exports + tests

- [ ] T015 Update `src/typescript/shared/ui/src/index.ts` and
  `src/typescript/shared/ui/src/native/index.ts` exports.
- [ ] T016 Add vitest coverage for all new components (web + native
  where applicable).

## Phase 6 -- Close-out

- [ ] T017 Run validation lane
  (`bun run --cwd src/typescript/shared/ui tsc --noEmit`,
  `bun run --cwd src/typescript/shared/ui test`).
- [ ] T018 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Per-channel adoption (slice 028).
- Behavior changes beyond styling source.
