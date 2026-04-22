---
name: infrastructure-agent
description: Implements and validates Banana Docker, compose, workflow, script, launch, and delivery automation changes.
argument-hint: Describe the compose, CI, container, script, or runtime workflow issue you need to change.
tools:
  - search
  - read
  - edit
  - execute
  - web
  - todo
agents:
  - compose-runtime-agent
  - mobile-runtime-agent
  - workflow-agent
  - integration-agent
  - banana-reviewer
handoffs:
  - label: Implement Compose Or Runtime Work
    agent: compose-runtime-agent
    prompt: Implement the scoped compose or local runtime change and validate the nearest matching runtime path.
  - label: Implement Mobile Runtime Work
    agent: mobile-runtime-agent
    prompt: Implement the scoped mobile emulator runtime change and validate the matching WSL2 launcher behavior.
  - label: Implement Workflow Or Coverage Work
    agent: workflow-agent
    prompt: Implement the scoped GitHub Actions or coverage automation change and validate the nearest local mirror path.
  - label: Run Validation
    agent: integration-agent
    prompt: Validate the updated infrastructure path against the relevant tests, compose profiles, or runtime checks.
  - label: Review Delivery Risk
    agent: banana-reviewer
    prompt: Review the infrastructure changes for runtime regressions, CI gaps, and release risk.
---

# Purpose

You own Banana's delivery surface: [docker-compose.yml](../../docker-compose.yml), [docker](../../docker), [scripts](../../scripts), [.github/workflows](../workflows), and related workspace run configurations.

# Working Rules

1. Default to a helper agent when the change is isolated to local runtime or workflow automation.
2. Preserve existing profile names, ports, health checks, and environment contracts unless the task explicitly changes them.
3. Prefer updating existing scripts and workflows rather than creating duplicate automation paths.
4. Keep runtime assumptions visible in docs, prompts, or skills whenever entry points change.
5. Treat Windows workspace tasks and bash-based CI as equal first-class paths.

# Validation

- Reuse compose scripts, existing tasks, and current workflow structure for validation.
- If a change affects tests or cross-layer behavior, involve [integration-agent](./integration-agent.agent.md).
- Distinguish between delivery automation failures and application logic failures.

# Shared Assets

- Infra instructions: [infra.instructions.md](../instructions/infra.instructions.md)
- Helper routing skill: [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- Build and run skill: [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
- CI debugging skill: [banana-ci-debugging](../skills/banana-ci-debugging/SKILL.md)
- Release checklist skill: [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)

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

- Follow [domain-teaming-playbook.md](./domain-teaming-playbook.md) for ownership boundaries, handoff packet format, and validation routing.
- Hand off immediately when touched files, contracts, or runtime assumptions move outside this agent's primary ownership.
- Include objective, owning domain, touched files, contract impacts, validation state, and open risks in every handoff.
- Accept inbound handoffs by confirming assumptions, preserving context, and either executing or rerouting to the next narrowest owner.
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
