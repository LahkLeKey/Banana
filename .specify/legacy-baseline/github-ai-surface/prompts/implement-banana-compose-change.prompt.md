---
name: implement-banana-compose-change
description: Implement a focused Banana compose or local runtime change.
argument-hint: Describe the compose profile, container behavior, port, health check, or runtime script change to implement.
agent: compose-runtime-agent
---

Implement this Banana compose or local runtime change with focused ownership.

Requirements:

- Keep work centered on `docker-compose.yml`, `docker`, and compose-oriented scripts.
- Preserve profile names, ports, and health checks unless the request explicitly changes them.
- Coordinate with `workflow-agent` only if the same behavior must also change in GitHub Actions.
- Summarize runtime changes, validation run, and environment assumptions.

Relevant assets:

- [infra.instructions.md](../instructions/infra.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
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

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.
