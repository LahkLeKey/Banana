---
name: Infrastructure And Delivery Standards
description: Guidance for Docker Compose, GitHub Actions, shell scripts, and local runtime orchestration.
applyTo: "{docker-compose.yml,docker/**/*,.github/workflows/**/*.yml,scripts/**/*.sh,scripts/**/*.bat,.vscode/**/*.json}"
---

# Infrastructure And Delivery Standards

- Use `compose-runtime-agent` for `docker-compose.yml`, `docker`, local runtime scripts, and health-check behavior.
- Use `workflow-agent` for `.github/workflows`, coverage automation, and CI artifact handling.
- Use `infrastructure-agent` only when local runtime and workflow surfaces need coordination.
- Prefer existing compose profiles and scripts instead of introducing parallel orchestration paths.
- Keep service contracts aligned with `docker-compose.yml`, especially ports, health checks, and shared environment variables.
- Reuse current CI stages in `.github/workflows/compose-ci.yml` for build, coverage, and runtime validation.
- Preserve cross-platform awareness: Windows task/build helpers exist alongside bash-based CI and compose scripts.
- If you change a workflow or script entry point, update the closest human-facing docs or prompt/skill references.

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.
