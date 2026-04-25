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

## 2. Verify specification quality gate

Review checklist status before planning continues:

- .specify/specs/004-trim-vibe-drift/checklists/requirements.md

## 3. Bootstrap planning artifacts

```bash
cd /c/Github/Banana
bash .specify/scripts/bash/setup-plan.sh --json
```

## 4. Validate drift-realignment design artifacts

Ensure these files exist and are populated:

- .specify/specs/004-trim-vibe-drift/plan.md
- .specify/specs/004-trim-vibe-drift/research.md
- .specify/specs/004-trim-vibe-drift/data-model.md
- .specify/specs/004-trim-vibe-drift/contracts/drift-realignment-workflow.md
- .specify/specs/004-trim-vibe-drift/quickstart.md

## 5. Enforce wiki freeze and canonical mirror parity

Run this when `.wiki` or `.specify/wiki/human-reference` content changes:

```bash
cd /c/Github/Banana
bash scripts/wiki-consume-into-specify.sh --dry-run
bash scripts/wiki-consume-into-specify.sh
```

## 6. Run AI contract validation when workflow surfaces change

Run this if planning implementation edits AI workflow contract files (instructions, skills, agents, workflows, or wiki sync mapping):

```bash
cd /c/Github/Banana
python scripts/validate-ai-contracts.py
```

## 7. Continue to task generation

Proceed with `/speckit.tasks` for implementation sequencing once planning artifacts are accepted.
