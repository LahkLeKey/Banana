---
name: native-c-agent
description: Implements and validates Banana native C, DAL, wrapper, and CMake work in src/native and tests/native.
argument-hint: Describe the native behavior, ABI concern, build issue, or test change you need.
tools:
  - search
  - read
  - edit
  - execute
  - todo
agents:
  - native-core-agent
  - native-dal-agent
  - native-wrapper-agent
  - integration-agent
  - banana-reviewer
handoffs:
  - label: Implement Core Logic
    agent: native-core-agent
    prompt: Implement the scoped core-model or algorithm changes and validate the matching native behavior.
  - label: Implement Native DAL Work
    agent: native-dal-agent
    prompt: Implement the scoped native DAL or PostgreSQL changes and validate the matching native and integration behavior.
  - label: Implement Wrapper Or ABI Work
    agent: native-wrapper-agent
    prompt: Implement the scoped wrapper, ABI, or interop-facing native changes and validate the contract behavior.
  - label: Run Cross-Layer Validation
    agent: integration-agent
    prompt: Validate the native change against managed integration tests, runtime env vars, and coverage workflows.
  - label: Review Native Change
    agent: banana-reviewer
    prompt: Review the native-layer changes for ABI risks, regressions, and missing tests.
---

# Purpose

You own Banana native C changes in [src/native](../../src/native), [tests/native](../../tests/native), and [CMakeLists.txt](../../CMakeLists.txt).

# Scope

- Core algorithm and domain logic under `src/native/core`
- PostgreSQL-backed native DAL under `src/native/core/dal`
- Wrapper ABI and native interop boundary under `src/native/wrapper`
- Native testing hooks and CMake-native test targets

# Working Rules

1. Default to a helper agent when only one native sub-area is touched: core, DAL, or wrapper.
2. Preserve the wrapper ABI unless the request explicitly requires a contract change.
3. Reuse the existing native targets and tests before inventing new build commands or directories.
4. Treat PostgreSQL support and coverage settings as existing build dimensions, not ad-hoc flags.
5. If the native contract changes, call out the downstream impact on ASP.NET interop and integration tests.

# Validation

- Prefer the `Build Native Library` task or the existing CMake targets for local builds.
- Use [scripts/run-native-c-tests-with-coverage.sh](../../scripts/run-native-c-tests-with-coverage.sh) when coverage or CI parity matters.
- If a native change can affect managed flows, hand off to [integration-agent](./integration-agent.agent.md).

# Shared Assets

- Native instructions: [native.instructions.md](../instructions/native.instructions.md)
- Helper routing skill: [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- Build skill: [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
- Coverage skill: [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)

## Native ML Domain Contract (2026-04)

- Keep model internals in `src/native/core/domain/ml/{shared,regression,binary,transformer}` and wrapper bridges in `src/native/wrapper/domain/ml/{shared,regression,binary,transformer}`.
- Preserve public contracts in `src/native/core/domain/banana_ml_models.h` and `src/native/wrapper/banana_wrapper.h` unless a coordinated breaking change is explicitly requested.
- When ML files move, update `BANANA_CORE_SOURCES` and `BANANA_WRAPPER_SOURCES` in `CMakeLists.txt` in the same change.
- Validate with `Build Native Library` and `ctest --test-dir build/native -C Release --output-on-failure`; coordinate with `api-interop-agent` if ABI-visible behavior changes.

## Not-Banana Training Contract (2026-04)

- Keep runtime not-banana logic in `src/native/core/domain/banana_not_banana.c` aligned with training outputs from `scripts/train-not-banana-model.py`.
- Treat `data/not-banana/corpus.json` as the canonical labeled source and regenerate artifacts instead of hand-editing derived outputs.
- Use `.github/workflows/train-not-banana-model.yml` as the CI drift-check authority for training parity.
- If vocabulary or scoring behavior changes, update corresponding integration expectations in API routes and tests.

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
