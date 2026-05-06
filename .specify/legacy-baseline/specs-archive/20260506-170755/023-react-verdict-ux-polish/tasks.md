# Tasks: React Verdict UX Polish

**Branch**: `023-react-verdict-ux-polish` | **Created**: 2026-04-26
**Status**: GATED. DO NOT EXECUTE until trigger fires.
**Trigger**: SPIKE 020 readiness packet ack + product greenlight to start
React adoption of canonical verdict copy.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` -> `023-react-verdict-ux-polish`.
- [x] T002 Author `README.md` execution tracker.

## Phase 2 -- Escalation panel component

- [x] T003 Add `src/typescript/shared/ui/src/components/EscalationPanel.tsx`
  rendering the embedding-summary panel per
  [copy-and-cue-baseline.md](../020-frontend-classifier-experience-spike/analysis/copy-and-cue-baseline.md).
- [x] T004 Export `EscalationPanel` from
  `src/typescript/shared/ui/src/index.ts`.
- [x] T005 Add vitest coverage in
  `src/typescript/shared/ui/src/components/EscalationPanel.test.tsx`
  for closed/open states + fetch error path.

## Phase 3 -- App rewire

- [x] T006 Add `fetchEmbeddingSummary(apiBaseUrl, sample)` to
  `src/typescript/react/src/lib/api.ts` calling
  `/ml/ensemble/embedding`.
- [x] T007 Replace ensemble result rendering in
  `src/typescript/react/src/App.tsx` with canonical copy + escalation
  cue from the baseline.
- [x] T008 Mount `EscalationPanel` next to the ensemble verdict
  surface; gate on `verdict.did_escalate`.

## Phase 4 -- Retry affordance

- [x] T009 Track last-submitted draft in `App.tsx` state (separate
  from current `ensembleInput`); render "Try again" button per
  baseline copy.
- [x] T010 On submit success or new-draft change, clear the retry
  button.

## Phase 5 -- Tests

- [x] T011 Add `src/typescript/react/src/App.test.tsx` covering all
  five baseline copy strings + escalation cue + retry flow.
- [x] T012 Extend `src/typescript/react/src/lib/api.test.ts` to
  cover `fetchEmbeddingSummary` happy + error paths.

## Phase 6 -- Close-out

- [x] T013 Run validation lane (`bun run --cwd src/typescript/shared/ui tsc --noEmit`,
  `bun run --cwd src/typescript/react tsc --noEmit`,
  `bun run --cwd src/typescript/react test`).
- [x] T014 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Tokens (SPIKE 021).
- Offline / queueing / history (SPIKE 022).
- Electron / RN.
