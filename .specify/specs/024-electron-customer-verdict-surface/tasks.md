# Tasks: Electron Customer Verdict Surface

**Branch**: `024-electron-customer-verdict-surface` | **Created**: 2026-04-26
**Status**: GATED.

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` -> `024-electron-customer-verdict-surface`.
- [x] T002 Author `README.md` execution tracker.

## Phase 2 -- Tray + global shortcut

- [x] T003 Add `src/typescript/electron/tray.js` with the menu
  entries from FR-024-01 using baseline copy.
- [x] T004 Add `src/typescript/electron/assets/tray-icon.png`
  (placeholder asset; design system replacement tracked under
  D-021-04 if/when icon tokens land).
- [x] T005 Register the global shortcut `Cmd/Ctrl+Shift+B` in
  `main.js` and route to the IPC channel created in T007.

## Phase 3 -- IPC + notifications

- [x] T006 Extend `src/typescript/electron/preload.js` to expose
  `window.banana.classifyClipboard()` and `window.banana.onVerdict(handler)`.
- [x] T007 Add `src/typescript/electron/ipc.js` defining the verdict
  IPC channel.
- [x] T008 Add `src/typescript/electron/notifications.js` that
  renders a native notification per FR-024-03 using baseline copy.

## Phase 4 -- Renderer wiring

- [x] T009 In `src/typescript/react/src/App.tsx`, subscribe to
  `window.banana.onVerdict` (when the bridge is present) so verdicts
  triggered from outside the renderer surface the same UI.

## Phase 5 -- Smoke + close-out

- [x] T010 Extend `src/typescript/electron/smoke.js` to assert the
  bridge surface (`classifyClipboard`, `onVerdict`).
- [x] T011 Run validation lane
  (`docker compose --profile apps run --rm electron-example npm run smoke`,
  `bun run --cwd src/typescript/react tsc --noEmit`).
- [x] T012 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Image ingest (D-020-01).
- Tokens (SPIKE 021).
- Offline / persistence (SPIKE 022; window state persistence ships in slice 030).
