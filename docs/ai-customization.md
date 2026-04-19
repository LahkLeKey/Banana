# Banana AI Customization Guide

This repository now includes a workspace-level GitHub Copilot customization layer for Banana's full SDLC.

## What Was Added

- Always-on repo guidance in `.github/copilot-instructions.md`
- File-scoped rules in `.github/instructions/`
- Custom agents in `.github/agents/`
- Reusable skills in `.github/skills/`
- Slash prompts in `.github/prompts/`

## Agent Lineup

- `banana-sdlc`: top-level orchestrator for end-to-end tasks
- `banana-planner`: read-only planning and impact analysis
- `banana-reviewer`: review-mode agent for findings, regressions, and release risk
- `native-c-agent`: coordinating native domain agent when core, DAL, and wrapper move together
- `native-core-agent`: focused helper for `src/native/core` logic and tests
- `native-dal-agent`: focused helper for `src/native/core/dal` and PostgreSQL-gated behavior
- `native-wrapper-agent`: focused helper for wrapper ABI, exports, and interop-facing native contracts
- `csharp-api-agent`: coordinating ASP.NET domain agent when pipeline and interop move together
- `api-pipeline-agent`: focused helper for controllers, services, middleware, DI, and pipeline steps
- `api-interop-agent`: focused helper for managed interop, data access, and native contract synchronization
- `react-agent`: coordinating frontend agent when React and Electron surfaces move together
- `react-ui-agent`: focused helper for React UI, client state, and typed frontend flows
- `electron-agent`: focused helper for Electron runtime, preload, and desktop bridge work
- `integration-agent`: cross-layer validation and escalation agent
- `test-triage-agent`: focused helper for failing tests, harness repairs, and fix-owner routing
- `infrastructure-agent`: coordinating delivery agent when compose/runtime and workflow surfaces move together
- `compose-runtime-agent`: focused helper for Docker Compose, local runtime scripts, and health checks
- `mobile-runtime-agent`: focused helper for Android emulator launch and iOS preview fallback runtime on Ubuntu WSL2/WSLg
- `workflow-agent`: focused helper for GitHub Actions, coverage automation, and CI job structure

## Shared Skills

- `banana-agent-decomposition`: helper routing and handoff selection before broad agents take ownership
- `banana-discovery`: architecture and entry-point mapping before implementation
- `banana-build-and-run`: correct build and local runtime selection
- `banana-test-and-coverage`: focused-to-broad validation and coverage guidance
- `banana-ci-debugging`: workflow, compose, and runtime failure triage
- `banana-mobile-runtime`: Ubuntu WSL2 mobile emulator launch and fallback guidance
- `banana-release-readiness`: final merge and release checklist

Each skill includes a `SKILL.md` file plus a referenced checklist or map so the agent can load more detail only when needed.

## Slash Prompts

- `/plan-banana-change`
- `/implement-banana-change`
- `/implement-banana-native-core-change`
- `/implement-banana-native-wrapper-change`
- `/implement-banana-api-pipeline-change`
- `/implement-banana-api-interop-change`
- `/implement-banana-react-ui-change`
- `/implement-banana-compose-change`
- `/implement-banana-mobile-runtime-change`
- `/implement-banana-workflow-change`
- `/validate-banana-change`
- `/debug-banana-test-failure`
- `/debug-banana-ci`
- `/debug-banana-mobile-runtime`
- `/review-banana-change`

These prompts route to the custom agents above and preload the right SDLC framing for common tasks.

## Typical Flows

### Feature Or Bug Work

1. Run `/plan-banana-change` with the requested outcome.
2. Prefer the helper-focused implementation prompt that matches the planned slice, or use `/implement-banana-change` if orchestration across helpers is still needed.
3. Run `/validate-banana-change` if the change crosses layers or affects runtime behavior.
4. Run `/review-banana-change` before merging.

### CI Or Runtime Failure

1. Run `/debug-banana-test-failure` when the failing stage is a test or harness problem, or `/debug-banana-ci` when the failing stage is workflow, compose, or runtime oriented.
2. If code changes are required, continue in the helper agent that owns the failing surface.
3. Finish with `/review-banana-change` for a release-risk pass.

## Repo-Specific Notes

- The backend architecture is centered on controller -> service -> pipeline -> native interop as documented in `docs/developer-onboarding.md`.
- Native and integration workflows often require `BANANA_PG_CONNECTION`.
- Managed runtime and integration workflows often require `BANANA_NATIVE_PATH`.
- React work should preserve Bun and `VITE_BANANA_API_BASE_URL`.
- Delivery automation should keep reusing `docker-compose.yml`, `scripts/*.sh`, and `.github/workflows/compose-ci.yml`.

## Maintenance

- Update the nearest customization file when a workflow changes instead of duplicating guidance in multiple places.
- Keep broad agents thin and move implementation guidance into the narrowest helper agent that can own the work.
- Use the Chat diagnostics view in VS Code if a custom agent, skill, prompt, or instruction file does not appear to load.
