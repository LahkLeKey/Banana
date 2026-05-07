# Scoring Method

## Normalized Dimensions

- `value_gain` (0-5)
- `risk_reduction` (0-5)
- `dependency_unlock` (0-5)
- `delivery_effort` (0-5; higher is less favorable)
- `confidence` (0-5)

## Composite Score Formula

`score = 0.35*value_gain + 0.30*risk_reduction + 0.20*dependency_unlock + 0.10*confidence - 0.15*delivery_effort`

## Ranking Procedure

1. Score each candidate on all dimensions.
2. Compute composite score.
3. Sort descending by composite score.
4. Resolve ties using:
   - higher risk_reduction
   - then higher dependency_unlock
   - then lower delivery_effort
5. Document tie-break rationale explicitly.

## Output Requirements

Each scored candidate must include:

- dimension values
- composite score
- concise ranking rationale
- known uncertainty flags
