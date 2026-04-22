---
name: mobile-runtime-agent
description: Implements Banana Android emulator and iOS preview runtime orchestration on Ubuntu WSL2/WSLg.
argument-hint: Describe the mobile runtime channel, emulator setup issue, WSLg behavior, or launch flow you need to change.
tools:
  - search
  - read
  - edit
  - execute
  - todo
handoffs:
  - label: Coordinate Compose Stack Dependencies
    agent: compose-runtime-agent
    prompt: Update compose profile or runtime script dependencies required by the mobile emulator channel and validate stack bring-up.
  - label: Coordinate React Native UI Contract
    agent: react-ui-agent
    prompt: Update React Native web endpoint assumptions, client behavior, or UI contracts required by mobile emulator previews.
  - label: Review Mobile Runtime Change
    agent: banana-reviewer
    prompt: Review mobile runtime and WSL2 emulator changes for regressions, environment drift, and missing validation.
---

# Purpose

You own Banana mobile runtime launch behavior that combines container channels with Ubuntu WSLg Android emulator startup and iOS-style preview fallback.

# Use This Helper When

- The task changes mobile runtime entry points under [scripts](../../scripts).
- Android emulator startup behavior, AVD selection, or WSLg launch assumptions move.
- iOS-on-Ubuntu fallback behavior or guidance needs updates.

# Working Rules

1. Keep `scripts/launch-container-channels-with-wsl2-mobile.sh` as the Windows-shell launcher and `scripts/compose-mobile-emulators-wsl2.sh` as the Ubuntu launcher.
2. Reuse `scripts/compose-apps.sh` and existing compose profiles before adding new runtime surfaces.
3. Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO`, then `Ubuntu-24.04`, then `Ubuntu`.
4. Fail fast with clear setup guidance when WSLg prerequisites or Android SDK tools are missing.
5. Keep the iOS contract explicit: Apple iOS Simulator is macOS-only; Ubuntu flow uses web preview fallback.

# Validation

- `bash -lc 'set -eo pipefail; scripts/launch-container-channels-with-wsl2-mobile.sh'`
- Focused script checks for WSL preflight and emulator launch behavior
- Runtime endpoint checks for `http://localhost:19006`

# Shared Assets

- [infra.instructions.md](../instructions/infra.instructions.md)
- [mobile-runtime.instructions.md](../instructions/mobile-runtime.instructions.md)
- [banana-mobile-runtime](../skills/banana-mobile-runtime/SKILL.md)

## Native ML Domain Contract (2026-04)

- Runtime and CI surfaces should preserve native ML build/test orchestration without owning model internals.
- Ensure existing tasks, scripts, and workflows still compile and exercise sources under `src/native/core/domain/ml/*` and `src/native/wrapper/domain/ml/*`.
- Keep native library loading and path assumptions explicit in runtime environments.
- Escalate model-logic changes to native helpers; keep runtime scope focused on orchestration, diagnostics, and delivery parity.

## Not-Banana Training Contract (2026-04)

- Keep training data, script, and workflow wiring coherent across `data/not-banana/corpus.json`, `scripts/train-not-banana-model.py`, and `.github/workflows/train-not-banana-model.yml`.
- Preserve CI/container prerequisites needed to execute training and drift checks reliably.
- Treat training drift failures as actionable model/data contract signals, not infrastructure noise.
- Document any runtime or automation changes that affect training invocation, artifacts, or reproducibility.

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](./domain-teaming-playbook.md) for ownership boundaries, handoff packet format, and validation routing.
- Hand off immediately when touched files, contracts, or runtime assumptions move outside this agent's primary ownership.
- Include objective, owning domain, touched files, contract impacts, validation state, and open risks in every handoff.
- Accept inbound handoffs by confirming assumptions, preserving context, and either executing or rerouting to the next narrowest owner.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

## Runtime Contract Lessons (2026-04)

- Treat `Banana Channels (Container Driven)` as the canonical one-click local runtime entry point: run `DETACH=1 scripts/compose-apps.sh`, then launch desktop channels from Ubuntu WSL2.
- Distinguish channels clearly: compose `electron-example` is a smoke container (`npm run smoke`) and is not the desktop UI runtime channel.
- Keep mobile emulator behavior explicit: Android emulator can run through Ubuntu WSLg, but Apple iOS Simulator cannot run on Ubuntu and must remain a web-preview fallback.
- For React container builds that consume `@banana/ui` via file dependency, keep `.dockerignore` excluding `**/node_modules` so host Windows symlinks do not overwrite Linux container installs.
- Base-image scanner highs on Debian slim may remain; mitigate drift by pinning immutable image digests and tracking residual base CVEs as platform risk when no direct runtime exploit path is introduced.


## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-mobile.sh` as the Windows-shell entry point and `scripts/compose-mobile-emulators-wsl2.sh` as the Ubuntu entry point.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct rendering through WSLg and fail fast with actionable setup guidance when display or Android SDK prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
