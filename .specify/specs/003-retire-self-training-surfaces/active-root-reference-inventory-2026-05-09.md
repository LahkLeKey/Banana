# Active-Root Self-Training Reference Inventory (2026-05-09)

## Scope

Active roots scanned for operational references:

- `.github/**`
- `scripts/**`
- `.specify/**` (excluding `.specify/legacy-baseline/**`)
- `docs/**`

Search terms:

- `autonomous self-training`
- `autonomous-self-training`
- `self-training`
- `autonomous-self-training-default-increments.json`
- `orchestrate-autonomous-self-training-cycle`

## Findings

### Operational Surfaces

No active operational references were found in:

- `.github/workflows/**`
- `scripts/**`
- `.specify/scripts/**`

### Active Non-Operational Mentions

References remain in feature-planning/history context only:

- `.specify/specs/003-retire-self-training-surfaces/spec.md`
- `.specify/specs/003-retire-self-training-surfaces/tasks.md`
- `.specify/specs/003-retire-self-training-surfaces/plan.md`
- `docs/release-notes/050-neuro-self-training.md` (historical release note)

### Catalog Dependency Check

- `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`
  - Not present in active-root usage references.
  - Active orchestration defaults currently resolve to `docs/automation/agent-pulse/sdlc-default-increments.json`.

### Post-Change Strict Verification

After retirement guard updates, a strict search excluding archival/spec-history files and the two intentional guard files produced no matches.

- Command result: no output, exit code `1` (no matches)
- Intentional guard files holding retired fragments:
  - `scripts/workflow-orchestrate-sdlc.sh` (negative-path guard constant)
  - `.specify/scripts/validate-ai-contracts.py` (retired-fragment detector list)

## Evidence Commands

```bash
git --no-pager grep -n -E "autonomous self-training|autonomous-self-training|self-training" -- .github scripts .specify docs ':(exclude).specify/legacy-baseline/**'
git --no-pager grep -n -E "autonomous-self-training-default-increments\.json|orchestrate-autonomous-self-training-cycle|autonomous-self-training" -- .github scripts .specify docs ':(exclude).specify/legacy-baseline/**'
git --no-pager grep -n "autonomous-self-training-default-increments.json" -- . ':(exclude).specify/legacy-baseline/**'
git --no-pager grep -n -E "orchestrate-autonomous-self-training-cycle|autonomous-self-training-default-increments\.json|autonomous self-training" -- .github/workflows scripts .specify/scripts docs/automation .specify/wiki .wiki ':(exclude).specify/legacy-baseline/**' ':(exclude).specify/specs/003-retire-self-training-surfaces/**' ':(exclude)docs/release-notes/**' ':(exclude).specify/scripts/validate-ai-contracts.py' ':(exclude)scripts/workflow-orchestrate-sdlc.sh'
```
