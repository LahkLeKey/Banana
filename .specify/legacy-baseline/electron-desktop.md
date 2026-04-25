# Legacy Baseline — Electron Desktop Runtime

**Paths**: `src/typescript/electron/`

## What exists

- Main: `main.js`. Preload: `preload.js`. Renderer wired in `renderer/`.
- Native bridge: `nativeBridge.js`, `nativeCli.js`. API client: `apiClient.js`.
- Smoke entry: `smoke.js` (used by compose `electron-example` smoke channel).
- Local launcher: `run-desktop.sh`.
- npm-managed (lockfile `package-lock.json`) — note: differs from React/api which use Bun.

## Hard contracts

- `Banana Channels (Container Driven)` is the canonical one-click local runtime; launched on Windows via `scripts/launch-container-channels-with-wsl2-electron.sh`.
- Strict direct container-to-WSLg rendering — fail fast when Display/Docker integration prerequisites are missing; integration preflight failure exit code is `42`.
- Container builds use `npm ci --omit=dev` + `CXXFLAGS=-fpermissive` for current native module compatibility.
- libGL/rendering details captured in `/memories/repo/electron-rendering-libgl.md`, runtime in `/memories/repo/electron-runtime.md`.
- Distro selection order: `BANANA_WSL_DISTRO` override → `Ubuntu-24.04` → `Ubuntu`.

## What works

- One-click launch via Windows shell wrapper.
- Smoke channel keeps `electron-example` healthy without launching the full UI.
- Native bridge cleanly separates IPC from main-process concerns.

## Vibe drift / pain points

- Mixed package managers (npm here vs Bun elsewhere) increases cognitive load.
- `nativeCli.js` and `nativeBridge.js` overlap — purpose split is implicit.
- Smoke vs UI channel distinction has historically caused confusion (smoke was mistaken for desktop UI runtime).
- libGL fixes are scattered between Dockerfile and runtime scripts.
- Base-image scanner highs on Debian slim require pinned digests — drift risk.

## Cross-domain dependencies

- Consumes: `009-react-shared-ui` for renderer, `007-aspnet-pipeline` for HTTP.
- Consumed by: `012-compose-runtime` (electron Dockerfile + service).

## v1 entry points to preserve in v2

- `scripts/launch-container-channels-with-wsl2-electron.sh` Windows entry point
- `scripts/compose-electron-desktop-wsl2.sh` Ubuntu entry point
- Exit code `42` for integration preflight failures
- WSLg direct rendering contract
