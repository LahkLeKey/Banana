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
- `native-c-agent`: native C, DAL, wrapper, and CMake work
- `csharp-api-agent`: ASP.NET controllers, services, pipeline, and interop work
- `react-agent`: existing frontend and Electron implementation agent
- `integration-agent`: tests, coverage, compose validation, and cross-layer triage
- `infrastructure-agent`: Docker, scripts, compose, CI, and runtime automation
- `banana-reviewer`: review-mode agent for findings, regressions, and release risk

## Shared Skills

- `banana-discovery`: architecture and entry-point mapping before implementation
- `banana-build-and-run`: correct build and local runtime selection
- `banana-test-and-coverage`: focused-to-broad validation and coverage guidance
- `banana-ci-debugging`: workflow, compose, and runtime failure triage
- `banana-release-readiness`: final merge and release checklist

Each skill includes a `SKILL.md` file plus a referenced checklist or map so the agent can load more detail only when needed.

## Slash Prompts

- `/plan-banana-change`
- `/implement-banana-change`
- `/validate-banana-change`
- `/debug-banana-ci`
- `/review-banana-change`

These prompts route to the custom agents above and preload the right SDLC framing for common tasks.

## Typical Flows

### Feature Or Bug Work

1. Run `/plan-banana-change` with the requested outcome.
2. Run `/implement-banana-change` to execute the work.
3. Run `/validate-banana-change` if the change crosses layers or affects runtime behavior.
4. Run `/review-banana-change` before merging.

### CI Or Runtime Failure

1. Run `/debug-banana-ci` with the failing job, compose profile, or terminal output.
2. If code changes are required, continue in `infrastructure-agent`, `integration-agent`, or the affected domain agent.
3. Finish with `/review-banana-change` for a release-risk pass.

## Repo-Specific Notes

- The backend architecture is centered on controller -> service -> pipeline -> native interop as documented in `docs/developer-onboarding.md`.
- Native and integration workflows often require `BANANA_PG_CONNECTION`.
- Managed runtime and integration workflows often require `BANANA_NATIVE_PATH`.
- React work should preserve Bun and `VITE_BANANA_API_BASE_URL`.
- Delivery automation should keep reusing `docker-compose.yml`, `scripts/*.sh`, and `.github/workflows/compose-ci.yml`.

## Maintenance

- Update the nearest customization file when a workflow changes instead of duplicating guidance in multiple places.
- Keep agent scopes narrow and prefer handoffs over giving every agent full repository authority.
- Use the Chat diagnostics view in VS Code if a custom agent, skill, prompt, or instruction file does not appear to load.
