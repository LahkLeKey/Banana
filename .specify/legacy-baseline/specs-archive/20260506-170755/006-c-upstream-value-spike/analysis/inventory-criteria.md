# Candidate Inventory Criteria

## Inclusion Criteria

A candidate is included when all conditions hold:

1. It is primarily a native C opportunity under `src/native/core` or `src/native/wrapper`.
2. It has plausible upstream value in at least one of API/UI/delivery domains.
3. It can be described as a follow-up story slice with bounded scope.
4. It has enough context in current repo artifacts to estimate value and risk.

## Exclusion Criteria

Exclude candidates that are:

1. Purely cosmetic refactors with no measurable upstream effect.
2. Broad rewrites without bounded incremental value.
3. Dependent on unavailable external systems with no mitigation path.
4. Duplicative of already-completed deliverables in 004/005.

## Inventory Constraints

- Minimum candidate count: 5.
- At least one candidate should emphasize dependency unlock.
- At least one candidate should emphasize risk reduction.
- Keep each candidate to one story-ready scope boundary.
