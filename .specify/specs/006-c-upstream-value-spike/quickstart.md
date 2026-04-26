# Quickstart: C Upstream Value Reassessment SPIKE

## Purpose

Run a fast, repeatable SPIKE workflow that produces ranked native C opportunity recommendations and story-ready handoff packets.

## Prerequisites

- Repository root: `C:/Github/Banana`
- 006 spec selected in `.specify/feature.json`
- Access to current native/API/test documentation and specs

## 1. Confirm active feature pointer

```bash
cd /c/Github/Banana
cat .specify/feature.json
```

Expected `feature_directory`:

- `.specify/specs/006-c-upstream-value-spike`

## 2. Review source context for candidate discovery

Use existing repository artifacts as input:

- `src/native/core/`
- `src/native/wrapper/`
- `src/c-sharp/asp.net/`
- `src/typescript/api/`
- `tests/native/`, `tests/unit/`, `tests/e2e/`

## 3. Build candidate inventory and scorecards

Create at least 5 candidates and score each with the normalized dimensions defined in `contracts/spike-contracts.md`.

## 4. Rank candidates and map upstream impact

For top-ranked candidates, document:

- impacted domains
- contract/env assumptions
- required validation lanes
- key risks

## 5. Produce follow-up readiness packets

Create story-ready packets for top 3 candidates including bounded scope and acceptance intent.

## 6. Validate artifact completeness

Confirm all planning artifacts exist:

```bash
cd /c/Github/Banana
ls .specify/specs/006-c-upstream-value-spike
ls .specify/specs/006-c-upstream-value-spike/contracts
ls .specify/specs/006-c-upstream-value-spike/checklists
ls .specify/specs/006-c-upstream-value-spike/analysis
```

## 7. Exact review commands used in execution

```bash
cd /c/Github/Banana

# Confirm feature context and available docs
.specify/scripts/bash/check-prerequisites.sh --json --require-tasks --include-tasks

# Inspect native and upstream source surfaces (rg not required)
find src/native/core src/native/wrapper -type f | sort
find src/c-sharp/asp.net/Controllers src/c-sharp/asp.net/NativeInterop tests/native tests/unit tests/e2e -type f \
	\( -name '*.cs' -o -name '*.c' -o -name '*.h' \) | grep -v '/bin/' | grep -v '/obj/' | sort

# Re-check analysis completeness against expected file list
expected='candidate-template.md scorecard-rubric.md upstream-impact-template.md inventory-criteria.md scoring-method.md confidence-policy.md validation-lane-mapping-rules.md candidate-inventory.md native-surface-map.md candidate-scorecards.md ranked-recommendations.md ranking-rationale.md top-candidate-selection.md readiness-packet-1.md readiness-packet-2.md readiness-packet-3.md readiness-packets.md readiness-open-questions.md upstream-impact-matrix.md contract-impact-notes.md validation-lane-plan.md cross-domain-sequencing.md residual-risk-summary.md success-criteria-evidence.md'
missing=''
for f in $expected; do
	[[ -f .specify/specs/006-c-upstream-value-spike/analysis/$f ]] || missing="$missing $f"
done
[[ -z "$missing" ]] && echo "ANALYSIS_COMPLETENESS=PASS" || echo "ANALYSIS_COMPLETENESS=FAIL missing:$missing"
```

## Expected outcomes

- Candidate inventory with normalized scoring is complete.
- Ranked recommendations are transparent and auditable.
- Top 3 candidates are ready to convert into follow-up implementation stories.
