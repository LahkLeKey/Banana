# Tasks: Frontend Classifier Experience SPIKE

**Branch**: `020-frontend-classifier-experience-spike` | **Created**: 2026-04-26
**Type**: SPIKE -- investigation only, no production code changes.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` ->
  `020-frontend-classifier-experience-spike`.
- [x] T002 Author `README.md` execution tracker.

## Phase 2 -- Channel inventory

- [x] T003 Inventory React (web) customer-facing entry points under
  `src/typescript/react/src/**`. Record verdict shape rendered,
  empty / error / loading states, and gaps in
  `analysis/channel-inventory.md`.
- [x] T004 Inventory Electron customer-facing surfaces under
  `src/typescript/electron/**` (today: smoke container only).
  Append to `analysis/channel-inventory.md`.
- [x] T005 Inventory React Native customer-facing surfaces under
  `src/typescript/react-native/**`. Append to
  `analysis/channel-inventory.md`.

## Phase 3 -- Journeys + canonical copy

- [x] T006 Identify the 3-5 cross-channel customer intents
  (paste->verdict, image->verdict, share-sheet->verdict, review,
  escalate). Record the primary/secondary channels per intent in
  `analysis/customer-journeys.md`.
- [x] T007 Pin canonical verdict copy, escalation cue, and error
  wording in `analysis/copy-and-cue-baseline.md`. The three follow-up
  slices MUST adopt these verbatim.

## Phase 4 -- Readiness packets

- [x] T008 Author `analysis/followup-readiness-react.md` (slice 023):
  in-scope files, validation commands (`bun run --cwd src/typescript/react tsc --noEmit`,
  vitest where applicable), estimated task count (<=20).
- [x] T009 Author `analysis/followup-readiness-electron.md`
  (slice 024): in-scope files, validation commands
  (`docker compose --profile apps run --rm electron-example npm run smoke`,
  any new vitest target), estimated task count (<=20).
- [x] T010 Author `analysis/followup-readiness-react-native.md`
  (slice 025): in-scope files, validation commands
  (`bun run --cwd src/typescript/react-native tsc --noEmit`,
  expo doctor where applicable), estimated task count (<=20).
- [x] T011 Author `analysis/deferred-registry.md` -- any items
  pushed past slice 025 with explicit triggers (e.g. shared design
  system gaps -> SPIKE 021; offline / persistence gaps -> SPIKE 022).

## Phase 5 -- Close-out

- [x] T012 Update `README.md` to COMPLETE; mark all tasks `[x]`;
  cross-link slices 023, 024, 025 from this SPIKE's deliverables.

## Out of scope

- Writing any production frontend code (slices 023-025 own that).
- Shared design system evolution (SPIKE 021).
- Offline / persistence (SPIKE 022).
- API or native changes.
