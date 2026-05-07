# Candidate Scorecard Rubric

## Scoring Dimensions

Use integer values from 0 to 5.

- `value_gain`
- `risk_reduction`
- `dependency_unlock`
- `delivery_effort`
- `confidence`

## Anchor Definitions

### 0

- no measurable contribution or unknown signal

### 1

- minimal contribution, weak confidence

### 2

- modest contribution, limited leverage

### 3

- meaningful contribution with clear practical benefit

### 4

- strong contribution with broad and near-term benefit

### 5

- exceptional contribution with high strategic leverage

## Effort Interpretation

Higher `delivery_effort` is less favorable and subtracts from composite score.

- 0 = negligible effort
- 5 = very high effort/coordination

## Composite Formula

`score = 0.35*value_gain + 0.30*risk_reduction + 0.20*dependency_unlock + 0.10*confidence - 0.15*delivery_effort`

## Required Scorecard Fields

- candidate_id
- each normalized dimension
- composite_score
- ranking_rationale
