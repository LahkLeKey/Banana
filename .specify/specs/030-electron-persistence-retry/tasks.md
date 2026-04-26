# Tasks: Electron Persistence + Retry

**Branch**: `030-electron-persistence-retry` | **Created**: 2026-04-26
**Status**: GATED. Hard prerequisite: slice 029.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `030-electron-persistence-retry`.
- [ ] T002 Author `README.md` execution tracker.

## Phase 2 -- Window state persistence

- [ ] T003 Add `src/typescript/electron/storage.js` (main-process
  shim using `electron-store` for window state only).
- [ ] T004 Update `src/typescript/electron/main.js` to read/write
  window position + size on lifecycle events.

## Phase 3 -- Tray history wiring

- [ ] T005 Extend `src/typescript/electron/preload.js` to expose
  `window.banana.history` bridge (read-only `list()` proxy).
- [ ] T006 Extend `src/typescript/electron/ipc.js` with the history
  IPC channel.
- [ ] T007 Update `src/typescript/electron/tray.js` "Show last
  verdict" entry to call the history bridge and render baseline
  copy in a notification.

## Phase 4 -- Drain notification

- [ ] T008 Extend `src/typescript/electron/notifications.js` with a
  drain-success notification path (renderer signals via IPC when a
  queued request completes).
- [ ] T009 Wire renderer drain handler in `App.tsx` to emit the
  drain-success IPC.

## Phase 5 -- Smoke + close-out

- [ ] T010 Extend `src/typescript/electron/smoke.js` to verify the
  history bridge surface.
- [ ] T011 Run validation lane
  (`docker compose --profile apps run --rm electron-example npm run smoke`,
  `bun run --cwd src/typescript/react tsc --noEmit`).
- [ ] T012 Update README to COMPLETE; mark all tasks `[x]`.

## Out of scope

- Verdict storage in main process.
- Cross-device sync.
