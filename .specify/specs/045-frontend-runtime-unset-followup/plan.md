# Implementation Plan: Frontend Runtime Unset Follow-up

**Branch**: `041-compose-orchestrated-run-profiles` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/045-frontend-runtime-unset-followup/spec.md`

## Summary

Resolve the remaining Banana v2 frontend runtime defects where the page still shows `API base: <unset>` and a configuration error despite canonical compose startup. Tighten runtime resolution semantics and rendered-state handling so configuration errors appear only for true misconfiguration, while session bootstrap and chat availability recover correctly after remediation.

## Technical Context

**Language/Version**: TypeScript (React frontend), JavaScript (Electron preload contract context), Bash (compose/diagnostic scripts), JSON (VS Code launch/tasks), YAML (Compose)  
**Primary Dependencies**: Compose profile entry points (`scripts/compose-run-profile.sh`, `scripts/compose-profile-ready.sh`), frontend runtime resolver (`src/typescript/react/src/lib/api.ts`), UI state surfaces (`src/typescript/react/src/App.tsx`), drift diagnostics (`scripts/validate-frontend-config-drift.sh`)  
**Storage**: N/A  
**Testing**: Targeted frontend unit tests, compose runtime validation loops, rendered-state verification through VS Code integrated browser  
**Target Platform**: Windows + Docker Desktop + Ubuntu WSL2 local runtime contract with VS Code integrated browser workflow  
**Project Type**: Frontend runtime contract hardening and behavior-fix slice in monorepo  
**Performance Goals**: Canonical startup-to-usable chat state within current local dev loop expectations; diagnostics flow remains under ~2 minutes on warm environment  
**Constraints**: Preserve existing compose profile architecture and API-base env contract keys; no backend/native business behavior changes  
**Scale/Scope**: React runtime resolution + error-state rendering + session bootstrap behavior, with supporting diagnostics and documentation updates

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Gate I (Domain Core First): PASS. No native domain rule changes.
- Gate II (Layered Interop Contract): PASS. No controller/service/pipeline/native interop contract modifications.
- Gate III (Spec First Delivery): PASS. Spec and plan prepared before implementation.
- Gate IV (Verifiable Quality Gates): PASS. Plan requires rendered-state and diagnostics validation.
- Gate V (Documentation and Wiki Parity): PASS. Runtime behavior and remediation updates include corresponding docs/runbook updates.
- Gate VI (One-Window Interactive Validation): PASS. Rendered verification in VS Code integrated browser is mandatory for closure.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/045-frontend-runtime-unset-followup/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
└── tasks.md
```

### Source Code (repository root)

```text
src/typescript/react/src/
├── App.tsx
└── lib/
    ├── api.ts
    └── api.test.ts

docker-compose.yml
.vscode/
├── launch.json
└── tasks.json

scripts/
└── validate-frontend-config-drift.sh

.specify/wiki/human-reference/
└── Build-Run-Test-Commands.md
```

**Structure Decision**: Keep the fix inside frontend/runtime contract surfaces (React resolver + UI behavior + diagnostics/docs). Reuse existing compose and launch entry points instead of introducing new orchestration paths.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None | N/A | N/A |
