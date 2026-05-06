# 024 Electron Customer Verdict Surface -- Execution Tracker

**Branch**: `024-electron-customer-verdict-surface`
**Source SPIKE**: [020 Frontend Classifier Experience](../020-frontend-classifier-experience-spike/spec.md)
**Status**: COMPLETE (2026-04-26).

## Outcome

- Tray icon + menu (`Classify clipboard`, `Show last verdict`,
  `Quit Banana`) added via [tray.js](../../../src/typescript/electron/tray.js).
- Global shortcut `Cmd/Ctrl+Shift+B` registered in
  [main.js](../../../src/typescript/electron/main.js) routes to the
  same paste-classify path as the tray entry.
- IPC channels (`banana:classify-clipboard`, `banana:verdict`)
  defined as a frozen registry in [ipc.js](../../../src/typescript/electron/ipc.js)
  and exposed to the renderer through a pure `buildBridge` factory in
  [preload.js](../../../src/typescript/electron/preload.js).
- Native notifications via [notifications.js](../../../src/typescript/electron/notifications.js)
  reuse the baseline verdict copy from slice 023.
- React renderer subscribes to `window.banana.onVerdict` so verdicts
  triggered from outside the renderer surface the same UI.
- [smoke.js](../../../src/typescript/electron/smoke.js) rewritten as a
  pure `node`-runnable contract harness asserting module loads, the
  bridge factory shape, IPC channel constants, baseline notification
  copy, and `defaultClassify` POST shape against `/ml/ensemble/embedding`.

## Constraints

- Image ingest deferred (D-020-01).
- Tokens / theming deferred to SPIKE 021 / slices 026-028.
- Window-state persistence deferred to slice 030.
- Tray icon is a placeholder PNG; design system replacement tracked
  under D-021-04.

## Validation lane

The spec lists `docker compose --profile apps run --rm electron-example npm run smoke`,
but `electron-example` is registered under `profiles: ["electron"]` in
[docker-compose.yml](../../../docker-compose.yml). The runnable command is:

```
docker compose --profile electron run --rm electron-example npm run smoke
bun run --cwd src/typescript/react tsc --noEmit
```

Locally the smoke harness was verified directly with `node smoke.js`
under `src/typescript/electron`.
