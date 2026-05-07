# Launch Surface Matrix (G1)

Today's host-side launch surface, per channel.

| Channel       | Cwd                              | Command                        | Required env                                          | Prereqs                          |
| ------------- | -------------------------------- | ------------------------------ | ----------------------------------------------------- | -------------------------------- |
| API           | `src/typescript/api`             | `bun run dev`                  | `BANANA_NOT_BANANA_MODEL_PATH` (optional; empty -> default) | bun installed                    |
| React (web)   | `src/typescript/react`           | `bun run dev` (vite)           | `VITE_BANANA_API_BASE_URL` (default `http://localhost:8080`) | API running                      |
| Electron      | `src/typescript/electron`        | `node smoke.js` or `npm start` | -                                                     | (smoke) none; (start) electron deps installed |
| React Native  | `src/typescript/react-native`    | `bunx expo start --web`        | `EXPO_PUBLIC_BANANA_API_BASE_URL` (default `http://localhost:8080`) | API running                      |

## Container surface (out of scope for F5)

The reproducible container path is owned by:

- `scripts/launch-container-channels-with-wsl2-electron.sh`
- `scripts/launch-container-channels-with-wsl2-mobile.sh`
- `docker-compose.yml` (profiles: `apps`, `electron-example`, `android-emulator`, ...)

These remain the canonical reproducible runtime. F5 is host-side
developer convenience only.

## Observations

- Every frontend wants the API up first -> compound entries are
  the high-value F5 surface.
- All env vars have sensible defaults; no `.env.local` required to
  boot the happy path.
- Electron has two modes: `smoke.js` (CLI assertion harness, no
  window) and `npm start` (full window). F5 chooses `smoke` to
  stay friction-free in headless contexts; full-window debug is
  deferred to slice 035.
