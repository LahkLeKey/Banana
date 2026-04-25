# Quickstart: Specify-Driven Drift Realignment

## 1. Confirm active feature context

```bash
cd /c/Github/Banana
git rev-parse --abbrev-ref HEAD
cat .specify/feature.json
```

Expected context:

- Branch: 004-trim-vibe-drift
- Feature pointer: .specify/specs/004-trim-vibe-drift

## 2. Scope-first author flow

Before planning, ensure the feature spec contains:

- `## In Scope` with bounded outcomes
- `## Out of Scope` with explicit exclusions
- `## Assumptions` with explicit decision defaults

Run local scope enforcement:

```bash
cd /c/Github/Banana
bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/004-trim-vibe-drift/spec.md
```

## 3. Verify specification quality gate

Review checklist status before planning continues:

- .specify/specs/004-trim-vibe-drift/checklists/requirements.md

## 4. Bootstrap planning artifacts

```bash
cd /c/Github/Banana
bash .specify/scripts/bash/setup-plan.sh --json
```

## 5. Validate drift-realignment design artifacts

Ensure these files exist and are populated:

- .specify/specs/004-trim-vibe-drift/plan.md
- .specify/specs/004-trim-vibe-drift/research.md
- .specify/specs/004-trim-vibe-drift/data-model.md
- .specify/specs/004-trim-vibe-drift/contracts/drift-realignment-workflow.md
- .specify/specs/004-trim-vibe-drift/quickstart.md

## 6. Run local drift checks

Validate task traceability and drift categorization:

```bash
cd /c/Github/Banana
bash .specify/scripts/bash/validate-task-traceability.sh \
	--spec .specify/specs/004-trim-vibe-drift/spec.md \
	--tasks .specify/specs/004-trim-vibe-drift/tasks.md
```

Run the orchestration wrapper (spec boundaries + traceability + wiki parity):

```bash
cd /c/Github/Banana
bash .specify/scripts/bash/orchestrate-drift-realignment.sh
```

Record decisions for deferred/split/reject findings:

```bash
cd /c/Github/Banana
bash .specify/scripts/bash/record-realignment-decision.sh \
	--finding-id DF-001 \
	--decision-type defer \
	--rationale "Deferred to preserve current scope boundary" \
	--follow-up-destination ".specify/specs/005-v2-regeneration" \
	--decision-owner "banana-sdlc"
```

## 7. Enforce wiki freeze and canonical mirror parity

Run this when `.wiki` or `.specify/wiki/human-reference` content changes:

```bash
cd /c/Github/Banana
bash scripts/wiki-consume-into-specify.sh --dry-run
bash scripts/wiki-consume-into-specify.sh
```

## 8. Run AI contract validation when workflow surfaces change

Run this if planning implementation edits AI workflow contract files (instructions, skills, agents, workflows, or wiki sync mapping):

```bash
cd /c/Github/Banana
python scripts/validate-ai-contracts.py
```

## 9. Validation command matrix

| Validation surface | Command |
| --- | --- |
| Scope boundaries | `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec <spec.md>` |
| Task traceability | `bash .specify/scripts/bash/validate-task-traceability.sh --spec <spec.md> --tasks <tasks.md>` |
| End-to-end drift cycle | `bash .specify/scripts/bash/orchestrate-drift-realignment.sh` |
| Wiki allowlist + parity | `bash scripts/wiki-consume-into-specify.sh --dry-run` |
| AI contract checks | `python scripts/validate-ai-contracts.py` |

## 10. Continue to task generation

Proceed with `/speckit.tasks` for implementation sequencing once planning artifacts are accepted.
