# Build And Run Entry Points

## Native

- Workspace task: `Build Native Library`
- Script: `scripts/build-native.bat` on Windows
- Coverage build path: `scripts/run-native-c-tests-with-coverage.sh`
- Primary helpers: `native-core-agent`, `native-dal-agent`, `native-wrapper-agent`

## ASP.NET

- Workspace task: `Build Banana API`
- Full build script: `scripts/build-all.sh`
- Local run helper: `scripts/run-api.sh` when available in the current environment
- Primary helpers: `api-pipeline-agent`, `api-interop-agent`

## Frontend

- React dev: `bun run dev` in `src/typescript/react`
- React build: `bun run build` in `src/typescript/react`
- Electron smoke: `npm run smoke` in `src/typescript/electron`
- Primary helpers: `react-ui-agent`, `electron-agent`

## Compose And Runtime

- Full apps stack: `bash scripts/compose-apps.sh`
- Detached full apps stack: `DETACH=1 bash scripts/compose-apps.sh`
- Stop apps stack: `bash scripts/compose-apps-down.sh`
- Test profile: `bash scripts/compose-tests.sh`
- Runtime profile: `bash scripts/compose-runtime.sh`
- Electron desktop channel: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-electron.sh'`
- Mobile emulator channel: `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Primary helpers: `compose-runtime-agent`, `mobile-runtime-agent`, `workflow-agent`

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: launch from a Windows shell with `scripts/launch-container-channels-with-wsl2-electron.sh` (or the VS Code profile) so Docker Desktop + Ubuntu-24.04/Ubuntu WSL2 runtime contracts stay enforced.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.


## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
