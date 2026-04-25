# Drift Realignment Workflow

This workflow keeps feature planning centered on `.specify` and enforces drift-alignment review gates before implementation.

## Workflow File

- `.specify/workflows/drift-realignment/workflow.yml`

## Expected Inputs

- `spec`: natural-language feature description
- `integration`: integration name (default: `copilot`)

## Stage Sequence

1. `speckit.specify`
2. Scope-boundary review gate
3. `speckit.plan`
4. Traceability review gate
5. `speckit.tasks`
6. Task-drift review gate
7. Wiki-freeze and mirror-parity review gate

## Intended Outcome

- All planning orchestration is initiated from `.specify/workflows`
- Spec, plan, and tasks are produced before implementation starts
- Reviewers can reject drift at each gate before code execution
