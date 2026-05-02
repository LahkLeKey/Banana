# Implementation Plan: Autonomous Spec Drain Mode

**Branch**: `103-autonomous-spec-drain-mode` | **Date**: 2026-05-01 | **Spec**: `.specify/specs/103-autonomous-spec-drain-mode/spec.md`
**Input**: Feature specification from `.specify/specs/103-autonomous-spec-drain-mode/spec.md`

## Summary

Implement a new orchestration mode (`spec-drain`) that executes eligible specs end-to-end without human prompts until the backlog is exhausted or a deterministic stop condition is reached. The implementation will reuse existing orchestration scripts and workflows, add checkpointed run state, introduce failure-budget controls, and emit machine-readable evidence per processed spec.

## Technical Context

**Language/Version**: Bash, Python 3.11, GitHub Actions YAML  
**Primary Dependencies**: gh CLI, existing `.specify/scripts/*` orchestration helpers, `scripts/workflow-orchestrate-sdlc.sh`  
**Storage**: File-based run state under `artifacts/` and/or `.artifacts/`  
**Testing**: Existing contract validators plus new orchestration smoke integration lane  
**Target Platform**: GitHub Actions + local Windows/Git Bash operator path  
**Project Type**: Workflow automation and orchestration scripts  
**Performance Goals**: Process next runnable spec within 60 seconds of prior completion in CI context  
**Constraints**: Must respect required checks and protected-branch policies; no bypass of failing mandatory gates  
**Scale/Scope**: Full `.specify/specs/*` backlog drain, sequential v1 execution

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Existing safety contracts (required checks, policy gates, validation scripts) are preserved.
- No direct production deployment behavior is introduced.
- All autonomous actions remain auditable by persisted machine-readable evidence.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/103-autonomous-spec-drain-mode/
├── spec.md
├── plan.md
└── tasks.md
```

### Source Code (repository root)

```text
.github/workflows/
└── orchestrate-autonomous-spec-drain.yml      # new or updated entry workflow

scripts/
├── workflow-orchestrate-sdlc.sh               # extend with spec-drain mode
├── workflow-orchestrate-triaged-item-pr.sh    # reuse for per-spec PR lifecycle
└── workflow-spec-drain-state.sh               # new helper for checkpoint state

.specify/scripts/
└── validate-ai-contracts.py                   # reused gate in autonomous loop

artifacts/
└── orchestration/
    └── spec-drain/                            # run-state + evidence snapshots
```

**Structure Decision**: Keep implementation in existing workflow/script surfaces to minimize blast radius and preserve current operator contracts.

## Phase Plan

### Phase 0: Research and Risk Framing

- Confirm the runnable-spec selection rules and blocked-state taxonomy.
- Define failure-budget semantics and emergency stop precedence.
- Choose state file format (`json`) and stable schema for resumability.

### Phase 1: Design and Contracts

- Define state schema: queue pointer, completed specs, failed specs, retries, budget counters, run id, timestamps.
- Define evidence schema per spec: selected inputs, validation outputs, PR number, merge status.
- Define deterministic stop outcomes (`exhausted`, `budget_exceeded`, `policy_blocked`, `kill_switch`).

### Phase 2: Implementation

- Add `spec-drain` mode path in orchestration entrypoint.
- Add checkpoint read/write helpers and idempotent resume logic.
- Add autonomous loop with branch/PR/validation/merge orchestration using existing scripts.
- Add kill switch (`BANANA_AUTONOMY_STOP=true`) and workflow input override.

### Phase 3: Validation

- Add CI lane that executes dry-run drain over a test fixture subset of specs.
- Add regression check that ensures no duplicate processing across restart simulation.
- Publish run summary artifact and verify machine-readable schema.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| Stateful orchestration file | Required for crash-safe resume without duplicate spec processing | Stateless loop cannot guarantee idempotency across retries |
| New workflow entry mode | Needed to separate continuous autonomy from existing manual/scheduled flows | Overloading current workflow path would increase accidental activation risk |
