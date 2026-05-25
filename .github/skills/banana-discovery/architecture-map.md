# Banana Architecture Map

## Domains

- Native C: `src/native`, `tests/native`, `CMakeLists.txt`
- TypeScript API: `src/typescript/api`, `tests/integration`
- Frontend: `src/typescript/react`, `src/typescript/shared/ui`
- Delivery/runtime: `docker`, `scripts`, `docker-compose.yml`, `.github/workflows`

## Core Flow

- HTTP enters Fastify route modules under `src/typescript/api/src/routes` and `src/typescript/api/src/domains/**/routes.ts`
- Orchestration lives in TypeScript domain services and route handlers
- Ordered work runs through route -> service -> native interop boundaries
- Native interop crosses through `src/typescript/api/src/lib/anti-cheat-interop.ts` and related adapters
- Native implementation lives in `src/native/core`, `src/native/core/dal`, and `src/native/wrapper`

## Helper Routing

- `native-core-agent`: `src/native/core` and focused native tests
- `native-dal-agent`: `src/native/core/dal` and PostgreSQL-gated native behavior
- `native-wrapper-agent`: `src/native/wrapper` and downstream ABI coordination
- `api-pipeline-agent`: Fastify routes, services, middleware/plugins, and API bootstrap
- `api-interop-agent`: API/native interop and contract mapping
- `react-ui-agent`: `src/typescript/react/src`
- `compose-runtime-agent`: `docker-compose.yml`, `docker`, and local runtime scripts
- `mobile-runtime-agent`: mobile runtime scripts and Ubuntu WSL2/WSLg emulator launch behavior
- `workflow-agent`: `.github/workflows` and coverage automation
- `test-triage-agent`: failing tests, harness issues, and validation ownership mapping

## Key Contracts

- `BANANA_PG_CONNECTION` for PostgreSQL-backed native and integration flows
- `BANANA_NATIVE_PATH` for TypeScript API/native runtime loading
- `VITE_BANANA_API_BASE_URL` for React runtime API calls

## Existing Entry Points

- Native build task: `Build Native Library`
- API build task: `bun run --cwd src/typescript/api build`
- Compose stack task: `Start Compose Apps`
- Mobile emulator launcher: `scripts/launch-container-channels-with-wsl2-mobile.sh`
- Aggregate tests: `scripts/run-tests-with-coverage.sh`
- Native coverage gate: `scripts/run-native-c-tests-with-coverage.sh`
- CI workflow: `.github/workflows/compose-ci.yml`

## Native Engine Contract (2026-04)

- During planning, review, and triage, keep gameplay and simulation changes inside `src/native/engine` and coordinated API interop layers.
- Require explicit confirmation that public contracts in `src/native/wrapper/banana_wrapper.h` remain stable unless a breaking change is approved.
- When native files move, require coordinated `CMakeLists.txt` updates for native source lists and build targets.
- Route implementation to native helpers (`native-core-agent`, `native-wrapper-agent`, `native-c-agent`) and require native build plus `ctest` evidence.

## Shared Frontend Contract

- If a task touches src/typescript/react or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
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

- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.


## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../../ubuntu-wsl2-runtime-contract.md).
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
