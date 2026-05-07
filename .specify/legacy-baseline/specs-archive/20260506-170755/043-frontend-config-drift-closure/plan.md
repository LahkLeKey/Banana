# Implementation Plan: Frontend Config Drift Closure

**Branch**: `041-compose-orchestrated-run-profiles` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/043-frontend-config-drift-closure/spec.md`

## Summary

Close the remaining frontend runtime drift where the UI still renders `API base: <unset>` despite container-level env wiring appearing correct. Add source-of-truth runtime checks and diagnostics that validate the effective API base as resolved by frontend code paths (React Vite env + Electron preload bridge + React Native web env) and fail fast with actionable remediation when empty. Keep all launch flows anchored to existing Compose profiles and VS Code tasks/configurations.

## Technical Context

**Language/Version**: TypeScript (React app), JavaScript (Electron preload), JSON (VS Code launch/tasks), YAML (Compose), Bash (diagnostic scripts)
**Primary Dependencies**: Docker Compose profiles, Bun/Vite dev server, Electron preload bridge, existing `scripts/compose-*.sh` entry points
**Storage**: N/A
**Testing**: Compose runtime checks, script-driven diagnostic assertions, targeted React unit test extension for API-base resolution behavior
**Target Platform**: Windows + Docker Desktop + Ubuntu WSL2 local runtime contract
**Project Type**: Monorepo runtime/config contract hardening
**Performance Goals**: Diagnostics and recovery flow should complete in under 2 minutes on warm local environment
**Constraints**: Preserve existing profile architecture and Bun package-manager contracts; no backend behavior changes
**Scale/Scope**: 1 new spec slice touching frontend config surfaces across React, Electron, React Native web, VS Code launch/tasks, and docs

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Gate I (Domain Core First): PASS. No native-domain algorithm changes.
- Gate II (Layered Interop Contract): PASS. No controller/service/pipeline contract changes; only runtime config/diagnostic surfaces.
- Gate III (Spec First Delivery): PASS. `spec.md` and this `plan.md` created before implementation.
- Gate IV (Verifiable Quality Gates): PASS with explicit requirement for runtime-resolution checks and scriptable diagnostics.
- Gate V (Documentation and Wiki Parity): PASS. Runbook/wiki updates are in-scope for configuration drift remediation.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/043-frontend-config-drift-closure/
├── plan.md              # This file (specify plan command output)
├── research.md          # Phase 0 output (specify plan command)
├── data-model.md        # Phase 1 output (specify plan command)
├── quickstart.md        # Phase 1 output (specify plan command)
├── contracts/           # Phase 1 output (specify plan command)
└── tasks.md             # Phase 2 output (specify tasks command - NOT created by specify plan)
```

### Source Code (repository root)

```text
docker-compose.yml
.vscode/
├── launch.json
└── tasks.json

scripts/
├── compose-run-profile.sh
├── compose-profile-ready.sh
└── (new) frontend config drift diagnostic script

src/typescript/react/src/
├── App.tsx
└── lib/api.ts

src/typescript/electron/
└── preload.js

src/typescript/react-native/
└── (config/env wiring touchpoints as needed)

.specify/wiki/human-reference/
└── Build-Run-Test-Commands.md
```

**Structure Decision**: Keep all implementation work in frontend/runtime orchestration surfaces. Do not change API/native domain code. Add one canonical diagnostic flow plus minimal UI/error messaging reinforcement where API base resolves empty.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None | N/A | N/A |
