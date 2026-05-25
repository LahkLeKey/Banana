---
name: banana-build-and-run
description: Choose the correct Banana build, local run, or compose entry point based on the domain being changed.
argument-hint: Describe the area you changed and whether you need a build, local run, or full stack bring-up.
---

# Banana Build And Run

Use this skill to pick the right existing build or runtime entry point instead of inventing new commands.

## Build Selection Rules

1. Pick the entry point that matches the narrowest helper agent first, then widen only if multiple helpers are involved.
2. Native-only work should start from the CMake task or existing native coverage script.
3. TypeScript API work should reuse Bun scripts under `src/typescript/api` and explicit native/runtime env vars when running locally.
4. React work should use Bun scripts under `src/typescript/react`.
5. Cross-layer runtime work should prefer the compose scripts and profiles already defined in the repo.

## Runtime Rules

- Keep `BANANA_NATIVE_PATH` explicit for API work.
- Keep `BANANA_PG_CONNECTION` explicit for native-backed or integration flows.
- Use compose profiles when multiple services must move together.

## Resources

- Command matrix: [entry-points.md](./entry-points.md)
- Helper routing: [../banana-agent-decomposition/helper-matrix.md](../banana-agent-decomposition/helper-matrix.md)

## Native Engine Contract (2026-04)

- Runtime and CI surfaces should preserve native engine build/test orchestration without owning gameplay internals.
- Ensure existing tasks, scripts, and workflows still compile and exercise sources under `src/native/engine` and `src/native/wrapper`.
- Keep native library loading and path assumptions explicit in runtime environments.
- Escalate gameplay logic changes to native helpers; keep runtime scope focused on orchestration, diagnostics, and delivery parity.

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

- Keep mobile emulator channels explicit in Ubuntu WSL2: Android emulator may launch through WSLg when SDK tools are installed; Apple iOS Simulator remains macOS-only and must stay a web-preview fallback on Ubuntu.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.


## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-mobile.sh` as the Windows-shell entry point and `scripts/compose-mobile-emulators-wsl2.sh` as the Ubuntu entry point for mobile emulator channels.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
