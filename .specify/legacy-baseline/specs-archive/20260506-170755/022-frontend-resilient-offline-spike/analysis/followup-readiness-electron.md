# 022 -- Followup Readiness: Slice 030 (Electron Persistence + Retry)

Bounded scope packet for the Electron-specific resilience slice.

**Hard prerequisite**: slice 029 has landed.

## Goal

Adopt the shared resilience primitive in the Electron channel.
Electron uses the same IndexedDB adapter as the React renderer
(it's still Chromium), but adds desktop-specific surfaces:
window-position persistence, "show last verdict" tray menu entry
backed by `VerdictHistory.list()`, and a native notification on
queued-then-drained verdicts.

## In-scope files

- `src/typescript/electron/main.js` (window position persistence;
  tray menu wiring to history)
- `src/typescript/electron/preload.js` (expose
  `window.banana.history` bridge for renderer)
- `src/typescript/electron/ipc.js` (extend; NEW history channel)
- `src/typescript/electron/storage.js` (NEW; main-process storage
  shim using `electron-store` for window state -- separate concern
  from verdict history which lives in renderer IndexedDB)
- `src/typescript/electron/notifications.js` (NEW; native notify on
  drain-success)
- `src/typescript/electron/smoke.js` (extend smoke to verify the
  bridge surface)

## Out of scope

- Verdict storage in the main process (renderer IndexedDB suffices).
- Cross-device sync.
- Auto-update.

## Validation lane

```
docker compose --profile apps run --rm electron-example npm run smoke
bun run --cwd src/typescript/react tsc --noEmit
```

## Estimated task count

10-13 tasks. Buckets: setup (1) + window state persistence (2) +
tray history wiring (3) + drain notification (2) + IPC bridge (2) +
smoke extension (1) + close-out (2).

## Acceptance

- Closing and reopening the Electron app restores window position.
- Tray menu "Show last verdict" surfaces the canonical verdict copy
  for the most recent stored verdict.
- A queued-while-offline verdict surfaces a native notification when
  it drains successfully.
- Smoke check passes in compose.
