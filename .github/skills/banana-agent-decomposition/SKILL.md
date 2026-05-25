---
name: banana-agent-decomposition
description: Break Banana work into the narrowest helper agents and explicit handoffs instead of overloading broad domain agents.
argument-hint: Describe the request, touched files, and whether you need planning, implementation, validation, or review.
---

# Banana Agent Decomposition

Use this skill when a Banana request is broad, crosses subdomains, or would be safer if split into helper-sized slices.

## Routing Procedure

1. Start from the touched paths, contracts, and phase of work: plan, implement, validate, review, or debug.
2. Prefer the narrowest helper agent that clearly owns the files and risks.
3. Route server-authoritative gameplay contract work to `native-c-agent` + `api-domain-agent` before broad orchestration.
4. Use a parent domain agent only when two or more helpers in the same domain must move together.
5. Use `banana-sdlc` only for multi-phase or multi-domain orchestration.
6. Make handoffs explicit: helper name, owned files, acceptance criteria, and validation surface.

## Output

- Primary helper or coordinating agent
- Secondary helpers or validation agents
- Files or folders owned by each helper
- Ordered handoffs
- Validation owner per slice

## Resources

- Helper matrix: [helper-matrix.md](./helper-matrix.md)
- Discovery skill: [../banana-discovery/SKILL.md](../banana-discovery/SKILL.md)
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
