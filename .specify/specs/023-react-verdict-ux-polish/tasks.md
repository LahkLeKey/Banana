# Tasks: React Verdict UX Polish

**Branch**: `023-react-verdict-ux-polish` | **Created**: 2026-04-26
**Status**: GATED. DO NOT EXECUTE until trigger fires.
**Trigger**: SPIKE 020 readiness packet ack + product greenlight to start
React adoption of canonical verdict copy.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `023-react-verdict-ux-polish`.
- [ ] T002 Author `README.md` execution tracker.

## Phase 2 -- Escalation panel component

- [ ] T003 Add `src/typescript/shared/ui/src/components/EscalationPanel.tsx`
  rendering the embedding-summary panel per
  [copy-and-cue-baseline.md](../020-frontend-classifier-experience-spike/analysis/copy-and-cue-baseline.md).
- [ ] T004 Export `EscalationPanel` from
  `src/typescript/shared/ui/src/index.ts`.
- [ ] T005 Add vitest coverage in
  `src/typescript/shared/ui/src/components/EscalationPanel.test.tsx`
  for closed/open states + fetch error path.

## Phase 3 -- App rewire

- [ ] T006 Add `fetchEmbeddingSummary(apiBaseUrl, sample)` to
  `src/typescript/react/src/lib/api.ts` calling
  `/ml/ensemble/embedding`.
- [ ] T007 Replace ensemble result rendering in
  `src/typescript/react/src/App.tsx` with canonical copy + escalation
  cue from the baseline.
- [ ] T008 Mount `EscalationPanel` next to the ensemble verdict
  surface; gate on `verdict.did_escalate`.

## Phase 4 -- Retry affordance

- [ ] T009 Track last-submitted draft in `App.tsx` state (separate
  from current `ensembleInput`); render "Try again" button per
  baseline copy.
- [ ] T010 On submit success or new-draft change, clear the retry
  button.

## Phase 5 -- Tests

- [ ] T011 Add `src/typescript/react/src/App.test.tsx` covering all
  five baseline copy strings + escalation cue + retry flow.
- [ ] T012 Extend `src/typescript/react/src/lib/api.test.ts` to
  cover `fetchEmbeddingSummary` happy + error paths.

## Phase 6 -- Close-out

- [ ] T013 Run validation lane (`bun run --cwd src/typescript/shared/ui tsc --noEmit`,
  `bun run --cwd src/typescript/react tsc --noEmit`,
  `bun run --cwd src/typescript/react test`).
- [ ] T014 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Tokens (SPIKE 021).
- Offline / queueing / history (SPIKE 022).
- Electron / RN.
