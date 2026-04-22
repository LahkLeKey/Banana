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

- Keep public ML API declarations in `src/native/core/domain/banana_ml_models.h` and wrapper exports in `src/native/wrapper/banana_wrapper.h` stable unless a contract change is explicit.
- Keep model internals split under `src/native/core/domain/ml/{shared,regression,binary,transformer}`.
- Keep wrapper ML bridges split under `src/native/wrapper/domain/ml/{shared,regression,binary,transformer}`.
- When ML model source files move, update `BANANA_CORE_SOURCES` and `BANANA_WRAPPER_SOURCES` in `CMakeLists.txt` in the same change.
- Validate ML refactors with `Build Native Library` and `ctest --test-dir build/native -C Release --output-on-failure`.

## Not-Banana Training Contract (2026-04)

- Keep `data/not-banana/corpus.json` as the canonical labeled corpus for not-banana vocabulary training.
- Use `scripts/train-not-banana-model.py` to regenerate metrics and model artifacts.
- Use `.github/workflows/train-not-banana-model.yml` as the CI training and drift-check path.
- Track any vocabulary drift between training outputs and runtime classifiers explicitly, especially `src/native/core/domain/banana_not_banana.c` and `src/typescript/api/src/domains/not-banana/routes.ts`.

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

## Ubuntu WSL2 Reproducible Contract (2026-04)

- Enforce the standard Windows + Docker Desktop + Ubuntu (Microsoft Store) workflow described in [../../ubuntu-wsl2-runtime-contract.md](../../ubuntu-wsl2-runtime-contract.md).
- Keep `scripts/launch-container-channels-with-wsl2-mobile.sh` as the Windows-shell entry point and `scripts/compose-mobile-emulators-wsl2.sh` as the Ubuntu entry point.
- Preserve Ubuntu-first distro selection order: `BANANA_WSL_DISTRO` override, then `Ubuntu-24.04`, then `Ubuntu`.
- Preserve strict direct rendering through WSLg and fail fast with actionable setup guidance when display or Android SDK prerequisites are missing.
