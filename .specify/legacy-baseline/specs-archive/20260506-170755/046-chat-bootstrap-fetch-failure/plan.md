# Implementation Plan: Chat Bootstrap Fetch Failure Follow-up

**Branch**: `041-compose-orchestrated-run-profiles` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/046-chat-bootstrap-fetch-failure/spec.md`

## Summary

Fix chat bootstrap startup failures that currently surface as `Chat bootstrap failed: Failed to fetch` by enforcing explicit transport-vs-configuration error classification, deterministic runtime readiness checks, and recovery behavior that transitions chat/session back to ready state once runtime dependencies become healthy.

## Technical Context

**Language/Version**: TypeScript (React frontend), Bash (compose and readiness scripts), YAML (compose profile contract), Markdown (feature docs/contracts)
**Primary Dependencies**: React runtime API client and bootstrap flow, compose profile scripts (`scripts/compose-run-profile.sh`, `scripts/compose-profile-ready.sh`), existing runtime contract (`VITE_BANANA_API_BASE_URL`)
**Storage**: N/A
**Testing**: React unit/integration validation for bootstrap state handling, compose runtime/apps readiness checks, rendered verification in VS Code integrated browser
**Target Platform**: Windows + Docker Desktop + Ubuntu WSL2 local runtime contract with one-window VS Code validation
**Project Type**: Frontend runtime/bootstrap reliability slice in monorepo
**Performance Goals**: Bootstrap reaches ready state under healthy startup in current local dev loop, with no persistent fetch-failure dead-end
**Constraints**: Preserve canonical compose runtime/apps startup order, preserve `VITE_BANANA_API_BASE_URL` contract, avoid backend business logic changes outside availability checks
**Scale/Scope**: React bootstrap/error-state behavior plus supporting diagnostics/contracts for startup failure and recovery path

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Pre-Design Gate Check

- Gate I (Domain Core First): PASS. Scope is frontend bootstrap/error behavior; no native domain-rule changes.
- Gate II (Layered Interop Contract): PASS. No controller/service/pipeline/native interop contract changes.
- Gate III (Spec First Delivery): PASS. Active feature spec exists and plan is generated before implementation.
- Gate IV (Verifiable Quality Gates): PASS. Plan includes healthy-path and failure/recovery validation.
- Gate V (Documentation and Wiki Parity): PASS. Runtime/bootstrap behavior changes include contract docs in the same feature slice.
- Gate VI (One-Window Interactive Validation): PASS. Validation requires VS Code integrated browser rendered-state checks.

### Post-Design Gate Check

- Gate I: PASS after data model/contract design.
- Gate II: PASS after data model/contract design.
- Gate III: PASS after data model/contract design.
- Gate IV: PASS after quickstart/validation flow definition.
- Gate V: PASS after contract artifact creation.
- Gate VI: PASS after integrated-browser validation steps were documented.

No unresolved gate violations remain. If any gate regresses during implementation, planning output is invalid until corrected.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/046-chat-bootstrap-fetch-failure/
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
├── lib/
│   ├── api.ts
│   └── (bootstrap/runtime helpers)
└── (chat/bootstrap UI state surfaces)

scripts/
├── compose-run-profile.sh
└── compose-profile-ready.sh

docker-compose.yml
.vscode/
└── tasks.json
```

**Structure Decision**: Keep changes in frontend/runtime contract surfaces and existing compose readiness entry points. Reuse current scripts/tasks rather than adding new orchestration paths.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None | N/A | N/A |
