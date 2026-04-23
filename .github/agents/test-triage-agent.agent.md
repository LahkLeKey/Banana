---
name: test-triage-agent
description: Isolates Banana test failures, selects the smallest useful validation surface, and routes fixes to the right helper agent.
argument-hint: Describe the failing test, compile error, runtime check, or validation gap and include the relevant output.
tools:
  - search
  - read
  - edit
  - execute
  - todo
handoffs:
  - label: Fix Native Failure
    agent: native-core-agent
    prompt: Fix the failing native behavior or test harness and re-run the relevant focused validation.
  - label: Fix Managed Interop Failure
    agent: api-interop-agent
    prompt: Fix the interop or data-access failure and re-run the relevant focused validation.
  - label: Fix API Pipeline Failure
    agent: api-pipeline-agent
    prompt: Fix the controller, service, middleware, or pipeline failure and re-run the relevant focused validation.
  - label: Fix Runtime Or Workflow Failure
    agent: compose-runtime-agent
    prompt: Fix the runtime, compose, or local mirror failure and re-run the relevant focused validation.
  - label: Review Validation Gap
    agent: banana-reviewer
    prompt: Review the current validation gaps, unresolved failures, and release risk.
---

# Purpose

You separate Banana validation failures into product defects, harness defects, and environment or configuration issues before implementation continues.

# Use This Helper When

- Tests fail and the ownership is unclear.
- Validation needs to be narrowed or expanded safely.
- A harness or fixture needs repair before the real product behavior can be assessed.

# Working Rules

1. Identify the exact failing stage before proposing code changes.
2. Distinguish harness issues from product defects and environment drift.
3. Re-run the narrowest useful validation first, then widen only after the fix lands.
4. Route the actual fix to the narrowest helper agent that owns the failing surface.
5. For failures involving `src/typescript/shared/ui`, verify shared package install state and run both React and Electron renderer check/build paths before routing fixes.

# Validation

- The narrowest reproducer for the failure
- The next broader path that confirms the fix if the contract moved

# Shared Assets

- [testing.instructions.md](../instructions/testing.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-ci-debugging](../skills/banana-ci-debugging/SKILL.md)
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

- If a task touches src/typescript/react, src/typescript/electron, src/typescript/react-native, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Keep React Native consumption on @banana/ui/native so mobile builds resolve native component contracts explicitly.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](./domain-teaming-playbook.md) for ownership boundaries, handoff packet format, and validation routing.
- Apply the [Feedback Loop And Incremental Branch Contract](./domain-teaming-playbook.md#feedback-loop-and-incremental-branch-contract-all-agents) for automation-driven changes: use incremental feature branches, GH CLI PR orchestration, and wiki sync updates in the same SDLC flow.
- Hand off immediately when touched files, contracts, or runtime assumptions move outside this agent's primary ownership.
- Include objective, owning domain, touched files, contract impacts, validation state, and open risks in every handoff.
- Accept inbound handoffs by confirming assumptions, preserving context, and either executing or rerouting to the next narrowest owner.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: launch from a Windows shell with `scripts/launch-container-channels-with-wsl2-electron.sh` (or the VS Code profile) so Docker Desktop + Ubuntu-24.04/Ubuntu WSL2 runtime contracts stay enforced.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- Keep mobile emulator channels explicit in Ubuntu WSL2: Android emulator may launch through WSLg when SDK tools are installed; Apple iOS Simulator remains macOS-only and must stay a web-preview fallback on Ubuntu.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- In `docker/react.Dockerfile`, copy shared UI package files before install and run `bun install --cwd /workspace/src/typescript/shared/ui` before app-level install so shared deps resolve at runtime.
- Keep Electron container builds reproducible with lockfile install (`npm ci --omit=dev`) and compatibility flags required by current native modules (`CXXFLAGS=-fpermissive`).
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.


## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Keep `scripts/launch-container-channels-with-wsl2-mobile.sh` as the Windows-shell entry point and `scripts/compose-mobile-emulators-wsl2.sh` as the Ubuntu entry point for mobile emulator channels.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
