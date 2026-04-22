# Banana Helper Matrix

## Native Helpers

- `native-core-agent`: `src/native/core`, core models, algorithms, execution context, and focused native tests
- `native-dal-agent`: `src/native/core/dal`, PostgreSQL-gated native data access, payload shaping, and `BANANA_PG_CONNECTION` behavior
- `native-wrapper-agent`: `src/native/wrapper`, ABI exports, status codes, memory ownership, and managed interop coordination

## ASP.NET Helpers

- `api-pipeline-agent`: controllers, services, middleware, DI wiring, pipeline executor, and ordered steps in `src/c-sharp/asp.net`
- `api-interop-agent`: `NativeInterop`, `DataAccess`, managed status translation, marshalling, and native contract synchronization

## Frontend Helpers

- `react-ui-agent`: `src/typescript/react/src`, UI state, forms, layouts, components, and styles
- `electron-agent`: `src/typescript/electron`, desktop runtime, preload, and native bridge code

## Runtime And Validation Helpers

- `compose-runtime-agent`: `docker-compose.yml`, `docker`, runtime scripts, local stack bring-up, and health-check behavior
- `mobile-runtime-agent`: mobile emulator launch scripts, Ubuntu WSL2/WSLg preflight, Android AVD startup, and iOS-preview fallback behavior
- `workflow-agent`: `.github/workflows`, coverage automation, CI artifact handling, and job structure
- `test-triage-agent`: test failure isolation, harness fixes, validation expansion, and fix-owner routing

## Parent Agents

- `native-c-agent`: coordinate when core, DAL, and wrapper all move together
- `csharp-api-agent`: coordinate when pipeline and interop or data access move together
- `react-agent`: coordinate React plus Electron or broader frontend work
- `infrastructure-agent`: coordinate compose/runtime and workflow changes together
- `integration-agent`: validate across helpers and separate environment failures from product defects
- `banana-sdlc`: orchestrate multi-domain, multi-phase work
## Native ML Domain Contract (2026-04)

- During planning, review, and triage, ensure ML changes stay inside `src/native/core/domain/ml/{shared,regression,binary,transformer}` and `src/native/wrapper/domain/ml/{shared,regression,binary,transformer}`.
- Require explicit confirmation that public contracts in `src/native/core/domain/banana_ml_models.h` and `src/native/wrapper/banana_wrapper.h` remain stable unless a breaking change is approved.
- When ML files move, require coordinated `CMakeLists.txt` updates for `BANANA_CORE_SOURCES` and `BANANA_WRAPPER_SOURCES`.
- Route implementation to native helpers (`native-core-agent`, `native-wrapper-agent`, `native-c-agent`) and require native build plus `ctest` evidence.

## Not-Banana Training Contract (2026-04)

- Treat `data/not-banana/corpus.json`, `scripts/train-not-banana-model.py`, and `.github/workflows/train-not-banana-model.yml` as one coordinated contract.
- Require drift checks whenever vocabulary or classifier logic changes across native and API layers.
- Ensure downstream behavior stays aligned in `src/native/core/domain/banana_not_banana.c` and `src/typescript/api/src/domains/not-banana/routes.ts`.
- Flag missing training validation, stale artifacts, or undocumented threshold shifts as release risk.

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
