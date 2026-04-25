---
name: banana-mobile-runtime
description: Implement and debug Banana Android emulator and iOS preview launch flows on Ubuntu WSL2/WSLg.
argument-hint: Describe the mobile launcher behavior, emulator startup issue, or Ubuntu runtime contract you need.
---

# Banana Mobile Runtime

Use this skill when a request changes Android emulator startup, iOS preview fallback, or Windows-to-Ubuntu launcher behavior for Banana mobile channels.

## What To Do

1. Keep the two-stage launch path intact:
   - Windows shell: `scripts/launch-container-channels-with-wsl2-mobile.sh`
   - Ubuntu shell: `scripts/compose-mobile-emulators-wsl2.sh`
2. Reuse compose app startup through `scripts/compose-apps.sh` before emulator launch.
3. Validate Ubuntu preflight assumptions first: WSL2, WSLg mount, and reachable React Native web endpoint.
4. For Android, enforce deterministic AVD selection and boot timeout behavior with clear remediation when SDK tools are missing.
5. For iOS, keep the contract explicit: Apple iOS Simulator is macOS-only; Ubuntu path is web-preview fallback.
6. If behavior changes, update the nearest prompts and runtime contract docs.

## Resources

- Runbook: [runbook.md](./runbook.md)
- Infrastructure guidance: [../../instructions/infra.instructions.md](../../instructions/infra.instructions.md)
- Mobile runtime guidance: [../../instructions/mobile-runtime.instructions.md](../../instructions/mobile-runtime.instructions.md)

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

- If a task touches src/typescript/react, src/typescript/electron, src/typescript/react-native, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Keep React Native consumption on @banana/ui/native so mobile builds resolve native component contracts explicitly.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
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

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [ubuntu-wsl2-runtime-contract.md](../../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-electron.sh` as the Windows-shell entry point and `scripts/compose-electron-desktop-wsl2.sh` as the Ubuntu entry point.
- Keep `scripts/launch-container-channels-with-wsl2-mobile.sh` as the Windows-shell entry point and `scripts/compose-mobile-emulators-wsl2.sh` as the Ubuntu entry point for mobile emulator channels.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct container-to-WSLg rendering behavior and fail fast with actionable setup guidance when display or Docker integration prerequisites are missing.
- Treat missing Ubuntu Docker server/socket as an environment contract failure first; surface clear remediation and keep exit code `42` for integration preflight failures.
