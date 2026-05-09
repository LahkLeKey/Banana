# Spec Orchestration Deliverables Contract

## Purpose

Define the canonical preflight, confidence heartbeat, and horizontal-scaling deliverables for Spec Kit orchestration under .specify.

## Core Deliverables

1. Extension health preflight runs before orchestration steps.
2. Confidence gating enforces an autonomous threshold of 80%.
3. Heartbeat evidence is appended for each major orchestration step.
4. Active feature resolution is sourced from .specify/feature.json.
5. Horizontal scaling artifacts include a shard plan and shard merge report.
6. Validators are rerun for boundary and traceability guarantees.

## Mandatory Command Sequence

1. .specify/scripts/bash/spec-extension-preflight.sh --update-first --json
2. .specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"
3. .specify/scripts/bash/validate-spec-boundaries.sh --spec <feature-spec>
4. .specify/scripts/bash/validate-task-traceability.sh --spec <feature-spec> --tasks <feature-tasks> --report <output>

## Confidence Policy

- Continue autonomously only when confidence is >= 80.
- If confidence is < 80, pause and request human input.
- Record both continue and pause decisions in heartbeat logs.

## DDD and SOLID Mapping

- Domain: confidence threshold policy, checkpoint rules, shard sequencing rules.
- Application: orchestration flow that composes preflight, planning, validation, and report generation.
- Infrastructure: shell scripts and artifact files that implement and persist orchestration evidence.
- Single responsibility: each script should own one concern (preflight, gate, heartbeat, planning, merging, validation).
- Dependency inversion: orchestrators compose existing scripts rather than reimplementing lower-level policy logic.

## Required Artifacts

- .specify/specs/<feature>/heartbeat-log.md
- artifacts/spec-validation/live-shard-plan.json
- artifacts/spec-validation/live-shard-merge-report.json
- artifacts/spec-validation/<feature>-traceability.md
