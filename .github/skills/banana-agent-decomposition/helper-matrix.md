# Banana Helper Matrix

## Native Helpers

- `native-core-agent`: `src/native/core`, core models, algorithms, execution context, and focused native tests
- `native-dal-agent`: `src/native/core/dal`, PostgreSQL-gated native data access, payload shaping, and `BANANA_PG_CONNECTION` behavior
- `native-wrapper-agent`: `src/native/wrapper`, ABI exports, status codes, memory ownership, and runtime interop coordination

## API Helpers

- `api-pipeline-agent`: Fastify routes, services, middleware/plugins, and ordered request orchestration in `src/typescript/api`
- `api-interop-agent`: API/native interop, status translation, and native contract synchronization

## Frontend Helpers

- `react-ui-agent`: `src/typescript/react/src`, UI state, forms, layouts, components, and styles

## Runtime And Validation Helpers

- `compose-runtime-agent`: `docker-compose.yml`, `docker`, runtime scripts, local stack bring-up, and health-check behavior
- `mobile-runtime-agent`: mobile emulator launch scripts, Ubuntu WSL2/WSLg preflight, Android AVD startup, and iOS-preview fallback behavior
- `workflow-agent`: `.github/workflows`, coverage automation, CI artifact handling, and job structure
- `technical-writer-agent`: wiki/doc structure and voice split, including `scripts/workflow-sync-wiki.sh`, `README.md`, and audience-specific navigation pages
- `value-risk-prioritization-agent`: backlog scoring, value-versus-risk prioritization, dependency-aware sequencing, and high-value execution slate generation
- `test-triage-agent`: test failure isolation, harness fixes, validation expansion, and fix-owner routing

## Parent Agents

- `native-c-agent`: coordinate when core, DAL, and wrapper all move together
- `api-domain-agent`: coordinate when pipeline and interop or data access move together
- `react-agent`: coordinate React and shared UI/frontend concerns
- `infrastructure-agent`: coordinate compose/runtime and workflow changes together
- `integration-agent`: validate across helpers and separate environment failures from product defects
- `banana-sdlc`: orchestrate multi-domain, multi-phase work
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
