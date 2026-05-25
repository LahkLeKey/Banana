# Release Readiness Checklist

## Code And Contract

- The changed area matches the intended domain scope
- The narrowest practical helper agent owned each implementation slice
- Cross-layer contracts were preserved or explicitly documented
- Native interop changes were paired with managed validation when needed

## Validation

- Relevant build task or script was run
- Relevant focused tests were run
- Broader integration, compose, or coverage checks were run when risk justified them

## Documentation And Ops

- User-facing or developer-facing workflow changes are documented near the changed entry point
- Prompt, skill, or instruction references remain accurate after the change
- Runtime env vars and health check expectations still match the compose and workflow configuration

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
