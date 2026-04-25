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

## End-to-end command sequence

```bash
cd /c/Github/Banana
bash .specify/scripts/bash/check-prerequisites.sh --json --paths-only
bash .specify/scripts/bash/validate-spec-boundaries.sh
bash .specify/scripts/bash/validate-task-traceability.sh
bash .specify/scripts/bash/orchestrate-drift-realignment.sh
```

Decision logging for deferred/split/reject findings:

```bash
cd /c/Github/Banana
bash .specify/scripts/bash/record-realignment-decision.sh \
	--finding-id DF-001 \
	--decision-type defer \
	--rationale "Moved to follow-up spec to preserve current slice boundary" \
	--follow-up-destination ".specify/specs/005-v2-regeneration" \
	--decision-owner "banana-sdlc"
```

## Troubleshooting

- `validate-spec-boundaries.sh` fails:
Add explicit non-empty `## In Scope`, `## Out of Scope`, and `## Assumptions` sections to the target spec.
- `validate-task-traceability.sh` reports `missing-traceability`:
Add `FR-###` or `[US#]` linkage to the task line, or explicitly record a drift decision.
- `validate-task-traceability.sh` reports `out-of-scope`:
Fix stale IDs or defer/split the work into the correct follow-up slice.
- Wiki parity gate fails:
Run `bash scripts/wiki-consume-into-specify.sh --dry-run` and resolve allowlist/parity diffs before approval.
