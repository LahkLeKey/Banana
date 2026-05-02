# Ranking Rationale: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Purpose**: Document how primary family rankings were determined and how ties and conflicts are resolved.

## Ranking Method

Families are ranked per domain using:
1. Domain mandate alignment: Does the family's defining strength match the domain's primary requirement?
2. Scorecard totals: Used as a secondary signal.
3. Tie-break policy: Applied when totals are equal.

## Tie-Break Policy

When two families score equally on total scorecard points for a domain, the tie is broken by the **defining dimension** of that domain:

| Domain | Defining Dimension (Tie-Break) |
|---|---|
| Left Brain | Calibration Fidelity |
| Right Brain | Classification Confidence Clarity |
| Full Brain | Contextual Reasoning Depth |

The family that scores H on the defining dimension wins the tie. If both score the same on the defining dimension, the assignment defaults to the user-directed or research-directed mapping.

## Ranking Decisions

### Left Brain: Regression over Binary

**Score tie**: Both Regression and Binary total 14 points.
**Tie-break applied**: Calibration Fidelity.
- Regression: H (3 points) — directly optimized for calibration.
- Binary: M (2 points) — requires post-hoc calibration.
**Winner**: Regression.
**Ruling**: Regression is Primary for Left Brain.

### Right Brain: Binary over Regression

**Score tie**: Both Binary and Regression total 14 points.
**Tie-break applied**: Classification Confidence Clarity.
- Binary: H (3 points) — optimized for classification decision thresholds.
- Regression: L (1 point) — not designed for categorical output.
**Winner**: Binary.
**Ruling**: Binary is Primary for Right Brain.

### Full Brain: Transformer (no tie)

**Score**: Transformer totals 11 points; Regression and Binary both total 14.
**Note**: Transformer has lower total because high validation complexity and explainability burden reduce its score.
**Domain mandate override applied**: Full Brain's mandate requires Contextual Reasoning Depth — the dimension only Transformer achieves at H.
**Ruling**: Transformer is Primary for Full Brain. Lower total is accepted because the mandate-defining dimension score overrides aggregate totals for this domain.

## Conflict Resolution Rule

If a future model family scores higher than the primary on total points but lower on the defining dimension:

1. The current primary mapping holds if the defining dimension gap is 1+ points.
2. If the new family matches or exceeds the primary on the defining dimension AND exceeds on total, the mapping is escalated to a SPIKE review — do not change unilaterally in implementation.
