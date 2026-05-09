---
name: go-copilot
description: Continue Banana Spec Kit orchestration autonomously with an 80% confidence heartbeat, mandatory human checkpoint below threshold, extension-health preflight, and horizontal scaling progression.
argument-hint: Optionally provide feature scope, shard target, or stop condition.
agent: banana-sdlc
---

Execute Banana Spec Kit orchestration with this contract:

## Core Contract

- Maintain a confidence heartbeat for each major orchestration step.
- Continue autonomously only when confidence is >= 80%.
- If confidence drops below 80%, stop and ask for human input before continuing.
- Keep constitution and Spec Kit templates aligned when confidence rules or orchestration behavior changes.
- Keep Spec Kit extensions healthy/up-to-date before orchestration steps.

## Required Preflight

1. Run extension health preflight:
- `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`

2. Record and gate confidence before execution:
- `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --notes "startup gate"`

## Execution Flow

1. Resolve active feature from `.specify/feature.json`.
2. Execute the next highest-priority incomplete tasks for the active feature.
3. For horizontal scaling slices, generate/update shard plan + merge report artifacts.
4. Re-run relevant validators:
- `validate-spec-boundaries.sh`
- `validate-task-traceability.sh`
5. Append heartbeat entry after each major step.
6. If confidence falls below 80% at any point, pause and request human input.

## Output Requirements

Return:
- Current confidence and whether autonomy continued or paused.
- Files changed.
- Validators/tests run and pass/fail status.
- Extension-health status.
- Next actionable step.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.
