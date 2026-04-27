# Implementation Plan: Frontend Slice Infra & Env Wiring

**Branch**: `041-compose-orchestrated-run-profiles` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/042-frontend-slice-infra-env/spec.md`

## Summary

The React web app shows `API base: <unset>` because `docker/react.Dockerfile` bakes `ENV VITE_BANANA_API_BASE_URL=http://api:8080` at image-build time. The hostname `api` is only resolvable inside the Docker network — not from the host browser. Since the container runs `bun run dev` (Vite dev server), the fix is: remove the baked `ENV` from the Dockerfile and supply the correct browser-resolvable URL (`http://localhost:8080`) through the `docker-compose.yml` `environment` block, which Vite picks up at server startup. The same pattern is applied to the Electron desktop and React Native web slices.

## Technical Context

**Language/Version**: YAML (Docker Compose), Dockerfile, TypeScript/Vite (React), JavaScript (Electron preload)
**Primary Dependencies**: Docker Compose, Vite dev server (`bun run dev`), Expo web (`bun run web`)
**Storage**: N/A
**Testing**: `docker compose exec <svc> printenv <var>` smoke check; `bun test` for React unit tests (no change expected)
**Target Platform**: Local developer workstation (Docker Desktop + host browser)
**Project Type**: Local dev environment configuration
**Performance Goals**: N/A
**Constraints**: `VITE_BANANA_API_BASE_URL` must resolve in the host browser; Vite dev server reads env at startup (not build time) so compose `environment` injection is sufficient
**Scale/Scope**: 3 docker-compose services, 1 Dockerfile line removal, 1 wiki doc update

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Gate I (Domain Core First): PASS. No native domain behavior changes.
- Gate II (Layered Interop Contract): PASS. No controller/service/pipeline/interop behavior changes.
- Gate III (Spec First Delivery): PASS. Feature has `spec.md` and this `plan.md` before implementation.
- Gate IV (Verifiable Quality Gates): PASS with targeted checks (`docker compose` env assertions and readiness checks).
- Gate V (Documentation and Wiki Parity): PASS. `.specify/wiki/human-reference/Build-Run-Test-Commands.md` is included in scope.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/042-frontend-slice-infra-env/
├── plan.md              # This file (specify plan command output)
├── spec.md              # Feature specification
└── tasks.md             # Phase 2 output (specify tasks)
```

### Source Code (repository root)

```text
docker-compose.yml
docker/
└── react.Dockerfile

.vscode/
├── launch.json
└── tasks.json

.specify/wiki/human-reference/
└── Build-Run-Test-Commands.md
```

**Structure Decision**: Keep the change constrained to infrastructure/runtime orchestration surfaces (`docker-compose.yml`, `docker/react.Dockerfile`, VS Code launch/tasks, and wiki command docs). No changes to ASP.NET or native code are required.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None | N/A | N/A |
