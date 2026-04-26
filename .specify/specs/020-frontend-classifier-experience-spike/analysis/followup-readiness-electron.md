# 020 -- Followup Readiness: Slice 024 (Electron Customer Verdict Surface)

Bounded scope packet for the Electron (desktop) implementation slice.

## Goal

Promote the Electron channel from a smoke container to a real
customer surface: tray icon, paste-classify global shortcut, and a
native notification on verdict completion. Reuses the React renderer
for the verdict panel UI; adds desktop-only entry points around it.

## In-scope files

- `src/typescript/electron/main.js` (tray + global shortcut + IPC)
- `src/typescript/electron/preload.js` (expose `classifyClipboard`,
  `onVerdict` bridge)
- `src/typescript/electron/tray.js` (NEW; tray icon + menu)
- `src/typescript/electron/ipc.js` (NEW; verdict IPC channel)
- `src/typescript/electron/assets/tray-icon.png` (NEW; design system
  may replace icon in slice 028 -- track as deferred)
- `src/typescript/react/src/App.tsx` (consume `window.banana.onVerdict`
  to surface verdicts triggered from outside the renderer)
- `src/typescript/electron/smoke.js` (extend to assert tray exists)

## Out of scope

- Drop-to-classify image ingest (deferred; Electron drag-and-drop
  surface is a separate slice if customer demand surfaces; the
  text-only verdict surface ships first).
- Tokens / theming (SPIKE 021).
- Offline / queueing (SPIKE 022).
- Auto-launch on login, multi-window, full menu bar redesign.

## Validation lane

```
docker compose --profile apps run --rm electron-example npm run smoke
bun run --cwd src/typescript/react tsc --noEmit
```

## Estimated task count

15-18 tasks. Task buckets: setup (2) + tray (3) + global shortcut (2)
+ IPC channel (3) + native notification (2) + renderer wiring (2) +
smoke extension (2) + close-out (2).

## Acceptance

- Tray icon visible; menu has "Classify clipboard" and "Show last
  verdict" entries using baseline copy.
- Global shortcut `Cmd/Ctrl+Shift+B` triggers paste-classify.
- Native notification renders the canonical verdict copy.
- Errors surface via the baseline error wording in a toast inside
  the renderer (not via native dialog).
- Smoke check passes in compose.
