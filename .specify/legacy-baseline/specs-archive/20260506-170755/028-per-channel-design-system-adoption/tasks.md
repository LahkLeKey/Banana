# Tasks: Per-Channel Design System Adoption

**Branch**: `028-per-channel-design-system-adoption` | **Created**: 2026-04-26
**Status**: GATED. Hard prerequisite: slices 026 + 027.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` -> `028-per-channel-design-system-adoption`.
- [x] T002 Author `README.md` execution tracker.

## Phase 2 -- React adoption

- [x] T003 Update `src/typescript/react/src/index.css` to load
  `@banana/ui/tokens/web.css`.
- [x] T004 Replace inline Tailwind colors / spacing in
  `src/typescript/react/src/App.tsx` verdict + error surfaces with
  v2 components from slice 027.
- [x] T005 Sweep remaining `src/typescript/react/src/**` for any
  remaining hex / px literals; replace or document exceptions.
- [x] T006 Update vitest snapshots if needed.

## Phase 3 -- React Native adoption

- [x] T007 Replace inline color hex literals in
  `src/typescript/react-native/index.tsx` with `tokens.color.*`.
- [x] T008 Replace inline literals in
  `src/typescript/react-native/screens/CaptureScreen.tsx`.
- [x] T009 Replace inline literals in
  `src/typescript/react-native/screens/VerdictScreen.tsx`.
- [x] T010 Sweep remaining RN sources for residual literals.

## Phase 4 -- Electron sanity

- [x] T011 Verify Electron tray icon asset path is unchanged or
  references token-derived assets (no-op if no icon-token chosen).

## Phase 5 -- Visual diff + close-out

- [x] T012 Run visual diff review for verdict / chat / ripeness
  screens (web + RN); confirm zero pixel-perceivable change.
- [x] T013 Run validation lane
  (`bun run --cwd src/typescript/react tsc --noEmit`,
  `bun run --cwd src/typescript/react test`,
  `bun run --cwd src/typescript/react-native tsc --noEmit`,
  `bun run --cwd src/typescript/shared/ui tsc --noEmit`,
  `docker compose --profile apps run --rm electron-example npm run smoke`).
- [x] T014 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- New components (slice 027).
- Dark mode (D-021-01).
