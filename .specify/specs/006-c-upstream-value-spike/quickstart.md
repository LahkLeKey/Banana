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
```

## Expected outcomes

- Candidate inventory with normalized scoring is complete.
- Ranked recommendations are transparent and auditable.
- Top 3 candidates are ready to convert into follow-up implementation stories.
