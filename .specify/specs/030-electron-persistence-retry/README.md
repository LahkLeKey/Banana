# Slice 030 — Electron Persistence + Retry — COMPLETE

Branch: `feature/030-electron-persistence-retry`

## What landed

- **`src/typescript/electron/storage.js`** (NEW) — main-process JSON
  shim using built-in `fs` (no `electron-store` dep) for window
  position + size. Pure factory `createWindowStateStore({readFile,
  writeFile, mkdir, filePath | getUserDataPath})` with `sanitize` +
  `DEFAULT_STATE` exports for smoke contract assertions.
- **`src/typescript/electron/main.js`** — restores persisted window
  position + size on `createWindow()`; persists on `resize`/`move`/
  `close`. Wires `setupHistoryIpc` + `setupDrainIpc`. Exposes
  `getCachedHistory` to the tray (FR-030-01, FR-030-04).
- **`src/typescript/electron/ipc.js`** — adds `historyUpdate` +
  `drainSuccess` channels and `setupHistoryIpc` / `setupDrainIpc`
  helpers. Renderer publishes its `VerdictHistory` snapshot to main
  (no main-process verdict storage; FR-030-04).
- **`src/typescript/electron/preload.js`** — bridge gains
  `history.publish(list)` + `notifyDrainSuccess(payload)`.
- **`src/typescript/electron/tray.js`** — "Show last verdict" reads
  the cached history snapshot and raises a native notification with
  canonical baseline copy via `verdictBody` (FR-030-02).
- **`src/typescript/electron/notifications.js`** — adds
  `notifyDrainSuccess` + `drainSuccessBody` paths (FR-030-03); body
  re-uses the slice 024 baseline copy strings.
- **`src/typescript/react/src/App.tsx`** — when running inside
  Electron (`window.banana` present), publishes the recent-verdicts
  snapshot to the main process and signals drain success after the
  online-listener queue drain. Web tab is unaffected.

## Validation lane

- `node smoke.js` (under `src/typescript/electron/`) — 9/9 ok
  (5 baseline + 4 slice-030 contract checks).
- `bunx tsc --noEmit` — clean across `react`.
- `bun test --cwd src/typescript/react` — 25/0.
- `bash scripts/validate-spec-tasks-parity.sh --all` — OK 005-031.
- Heavy compose smoke (`docker compose --profile apps run --rm
  electron-example npm run smoke`) intentionally deferred:
  `node smoke.js` exercises the same harness with the same
  contract assertions, which is the same code path the compose run
  invokes.

## Out of scope

- Verdict storage in the main process (renderer IndexedDB suffices).
- Cross-device sync, auto-update.
