# 020 -- Channel Inventory

Audit of customer-facing surfaces across the three frontend channels as
of 2026-04-26. Source files cited inline.

## React (web) -- `src/typescript/react/src/`

Single-screen app rendered by [App.tsx](../../../../src/typescript/react/src/App.tsx).

| Surface | Verdict shape | Empty state | Loading state | Error state |
|---------|---------------|-------------|---------------|-------------|
| `BananaBadge` (today count) | `count: number` from `/banana/summary` | `count=0` shows "(today)" tail | none | silently sets `null` |
| `BananaBadge variant="ensemble"` | `EnsembleVerdict` from `/ml/ensemble` | hidden until first verdict | none (badge is final state) | error written below form, badge stays at last verdict |
| Ensemble form | textarea + submit | placeholder only | button copy "Predicting..." | `<p class="text-xs text-red-700">` below form |
| Ripeness form | textarea + submit, renders `RipenessLabel` | none | button copy "Predicting..." | `<p class="text-xs text-red-700">` below form |
| Chat | `ChatMessageBubble` list, textarea + submit | "No chat messages yet." | session "initializing" | `<p class="text-xs text-red-700">` below form |

Gaps:

- No escalation cue: `EnsembleVerdict.status` (escalated vs non-escalated) is not surfaced visually beyond the badge variant.
- No retry affordance on any form; failed submits leave the input empty.
- No history beyond in-memory state (refresh wipes verdicts).
- Attention diagnostics gated on `VITE_BANANA_SHOW_ATTENTION=1` env var (developer flag, not customer surface).

## Electron (desktop) -- `src/typescript/electron/`

Today this is **not a customer surface**. Confirmed:

- [main.js](../../../../src/typescript/electron/main.js) creates a single `BrowserWindow` that loads `BANANA_ELECTRON_RENDERER_URL` (default `http://localhost:5173` -- the React dev server).
- [preload.js](../../../../src/typescript/electron/preload.js) exposes only `window.banana = { apiBaseUrl, platform }`.
- [smoke.js](../../../../src/typescript/electron/smoke.js) is a CI smoke check, not a customer entry.

Gaps (all of them; the channel has no first-class verdict surface yet):

- No tray icon, no global shortcut, no notification on verdict.
- No paste-classify or drop-to-classify entry.
- No verdict surface independent of the React renderer.
- No window menu actions for "classify clipboard" or "show last verdict".

## React Native (mobile) -- `src/typescript/react-native/`

Single-screen Expo app rendered by [index.tsx](../../../../src/typescript/react-native/index.tsx).

| Surface | Verdict shape | Empty state | Loading state | Error state |
|---------|---------------|-------------|---------------|-------------|
| `BananaBadge count={0}` | hard-coded `0`, never refreshed | always renders | none | n/a |
| `RipenessLabel value="yellow"` | hard-coded literal | always renders | none | n/a |
| Chat | `ChatMessageBubble` list, `TextInput` + button | "No chat messages yet." | "initializing" | error text below form |

Gaps:

- No verdict capture entry point: no camera, no share-sheet ingest, no clipboard paste, no image upload.
- No call to `/ml/ensemble` or `/predict-ripeness` exists in the app.
- The `BananaBadge` and `RipenessLabel` are stub renders, not live data.
- No offline cue (request will throw on submit if offline).

## Cross-channel summary

| Capability | React | Electron | RN |
|------------|-------|----------|----|
| Live ensemble verdict | YES | (proxies React via dev-server URL) | NO |
| Live ripeness verdict | YES | (proxies React) | NO |
| Live chat | YES | (proxies React) | YES |
| Escalation cue surfaced | NO (badge variant only) | NO | NO |
| Retry affordance | NO | NO | NO |
| Verdict history persistence | NO | NO | NO |
| Camera / share-sheet ingest | n/a | NO | NO |
| Tray / notification | n/a | NO | n/a |
