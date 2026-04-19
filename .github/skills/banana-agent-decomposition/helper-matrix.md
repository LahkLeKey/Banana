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
- `workflow-agent`: `.github/workflows`, coverage automation, CI artifact handling, and job structure
- `test-triage-agent`: test failure isolation, harness fixes, validation expansion, and fix-owner routing

## Parent Agents

- `native-c-agent`: coordinate when core, DAL, and wrapper all move together
- `csharp-api-agent`: coordinate when pipeline and interop or data access move together
- `react-agent`: coordinate React plus Electron or broader frontend work
- `infrastructure-agent`: coordinate compose/runtime and workflow changes together
- `integration-agent`: validate across helpers and separate environment failures from product defects
- `banana-sdlc`: orchestrate multi-domain, multi-phase work
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

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: run `DETACH=1 scripts/compose-apps.sh`, then launch local desktop Electron with `npm run start:ui` in `src/typescript/electron`.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.

