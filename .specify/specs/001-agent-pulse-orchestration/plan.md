# Implementation Plan: Agent Pulse Orchestration

**Branch**: `001-agent-pulse-orchestration` | **Date**: 2026-04-24 | **Spec**: `.specify/specs/001-agent-pulse-orchestration/spec.md`
**Input**: Feature specification from `.specify/specs/001-agent-pulse-orchestration/spec.md`

## Summary

Replace the large inline autonomous increment JSON in the workflow with a repository-managed default plan file, expand the default plan to cover a wider suite of agent-owned increments (including documentation ownership), and introduce a native C deterministic model that guides per-agent increment ordering and pulse repetition for management-visible commit cadence.

## Technical Context

**Language/Version**: C11 (native deterministic model), Bash (POSIX shell scripts), YAML (GitHub Actions), Python 3.11 (plan renderer utilities)  
**Primary Dependencies**: GitHub Actions runtime, native C compiler (`cc`/`gcc`/`clang`), GitHub CLI (`gh`) for PR orchestration, Python standard library  
**Storage**: Repository JSON/Markdown documentation under `docs/automation/` and orchestration artifacts under `artifacts/`  
**Testing**: `python scripts/validate-ai-contracts.py` plus local dry-run orchestration via `scripts/workflow-orchestrate-sdlc.sh`  
**Target Platform**: GitHub-hosted Ubuntu runners, developer Git Bash/PowerShell on Windows  
**Project Type**: Delivery automation workflow and scripting  
**Performance Goals**: Preserve bounded run behavior while handling expanded agent increments in the existing scheduled cadence  
**Constraints**: Keep `speckit-driven` governance labels, preserve required reviewer flow, keep existing not-banana training increments, maintain manual `incremental_plan_json` override behavior, and preserve human-readable vs AI-audit documentation split for management visibility  
**Scale/Scope**: Workflow env and script-driven plan parsing, native deterministic model source + CLI + tests, deterministic plan renderer scripts, and management-facing docs index

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- **Domain Core First**: Pass. Change is delivery/runtime only; no domain rule migration into upper layers.
- **Layered Interop Contract**: Pass. No controller/service/pipeline/native contract changes.
- **Spec First Delivery**: Pass. Feature uses Spec Kit `spec -> plan -> tasks` before implementation.
- **Verifiable Quality Gates**: Pass. Validation includes AI contract check and local orchestration dry run.
- **Documentation and Wiki Parity**: Pass. This feature explicitly adds management-facing docs and keeps wiki sync path in orchestration.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/001-agent-pulse-orchestration/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
└── tasks.md
```

### Source Code (repository root)

```text
.github/workflows/
└── orchestrate-autonomous-self-training-cycle.yml

scripts/
├── build-deterministic-agent-pulse-plan.py
├── generate-deterministic-agent-pulse-plan.sh
├── workflow-orchestrate-sdlc.sh
└── record-agent-pulse-activity.py

src/native/core/domain/
├── banana_agent_pulse_model.c
├── banana_agent_pulse_model.h
└── banana_agent_pulse_model_cli.c

tests/native/
└── banana_agent_pulse_model_tests.c

docs/
└── automation/
    └── agent-pulse/
        ├── README.md
        ├── Human-Reading-Guide.md
        ├── AI-Audit-Trails.md
        └── agents/
            └── *.md

artifacts/
└── sdlc-orchestration/

.specify/specs/001-agent-pulse-orchestration/contracts/
└── autonomous-increment-plan.md
```

**Structure Decision**: Keep all behavior in delivery/runtime automation surfaces (`.github/workflows`, `scripts`, and `docs`) while preserving existing SDLC orchestration entry points.

## Complexity Tracking

No constitution violations expected.
