# Implementation Plan: One-Window Interactive Baseline

**Branch**: `041-compose-orchestrated-run-profiles` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/044-one-window-interactive-baseline/spec.md`

## Summary

Institutionalize a one-window frontend validation baseline so developers use the VS Code integrated browser with compose-backed runtime as the default closure path for React/Electron UX work. The implementation updates governance, operational guidance, and Spec Kit tasking standards so rendered behavior checks become mandatory and repeatable, reducing drift that previously escaped env/container-only verification.

## Technical Context

**Language/Version**: Markdown/JSON documentation contracts; Bash task entry points; existing TypeScript/Electron runtime flows remain unchanged
**Primary Dependencies**: VS Code integrated browser workflow, Compose profile scripts (`scripts/compose-run-profile.sh`, `scripts/compose-profile-ready.sh`), Spec Kit artifacts under `.specify/`
**Storage**: N/A
**Testing**: Manual interactive validation loop + existing drift diagnostics (`scripts/validate-frontend-config-drift.sh`) + checklist evidence in spec tasks
**Target Platform**: Windows host with Docker Desktop + Ubuntu WSL2 runtime contract and VS Code workspace flow
**Project Type**: Process/governance hardening slice across documentation and workflow contracts
**Performance Goals**: One-window startup + rendered validation + teardown should remain within current developer loop expectations (typically under 5 minutes on warm environment)
**Constraints**: Preserve existing compose profile architecture and Bun-based frontend runtime contracts; avoid unrelated product behavior changes
**Scale/Scope**: Frontend/Electron validation lifecycle across constitution, Copilot guidance, runbook, and Spec Kit task execution conventions

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Gate I (Domain Core First): PASS. No native business logic changes.
- Gate II (Layered Interop Contract): PASS. No controller/service/pipeline/native interop contract modifications.
- Gate III (Spec First Delivery): PASS. Spec created before planning/tasks.
- Gate IV (Verifiable Quality Gates): PASS. Plan requires rendered validation evidence and repeatable checks.
- Gate V (Documentation and Wiki Parity): PASS. Governance and runbook updates are part of this slice.
- Gate VI (One-Window Interactive Validation): PASS by design; this feature operationalizes the principle.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/044-one-window-interactive-baseline/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
└── tasks.md
```

### Source Code (repository root)

```text
.specify/memory/constitution.md
.github/copilot-instructions.md
.specify/wiki/human-reference/Build-Run-Test-Commands.md

.vscode/
├── launch.json
└── tasks.json

scripts/
├── compose-run-profile.sh
├── compose-profile-ready.sh
└── validate-frontend-config-drift.sh

src/typescript/react/
src/typescript/electron/
```

**Structure Decision**: Keep this slice documentation-first with minimal runtime-touch validation references. No new executable runtime components are introduced; this work binds existing launch/diagnostic entry points into a mandatory one-window closure contract.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| None | N/A | N/A |
