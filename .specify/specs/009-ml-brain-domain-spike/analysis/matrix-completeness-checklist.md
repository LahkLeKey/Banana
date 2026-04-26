# Matrix Completeness Checklist (US2)

**Date**: 2026-04-26  
**Purpose**: Gate to confirm the tradeoff matrix covers all families across all normalized dimensions.

## Checklist

### Coverage Completeness

- [ ] Regression row covers all 6 dimensions: Output Determinism, Calibration Fidelity, Classification Confidence Clarity, Contextual Reasoning Depth, Explainability Burden, Validation Complexity
- [ ] Binary row covers all 6 dimensions
- [ ] Transformer row covers all 6 dimensions
- [ ] No cells are blank or marked TBD (per comparison-dimensions.md rule)

### Score Quality

- [ ] All scores use H/M/L convention from comparison-dimensions.md
- [ ] All scores have accompanying rationale notes
- [ ] Planned-grade scores annotated with [M] confidence per confidence-policy.md

### Ranking Completeness

- [ ] Domain-fit scorecard exists for Left Brain
- [ ] Domain-fit scorecard exists for Right Brain
- [ ] Domain-fit scorecard exists for Full Brain
- [ ] Ranking rationale documented with tie-break policy
- [ ] Uncertainty impacts on ranking are recorded

### Cross-Check

- [ ] Matrix scores are consistent with brain-domain-definitions.md reasoning styles
- [ ] Matrix scores are consistent with model-family-mappings.md primary assignments
- [ ] Any score that would suggest a different primary mapping is explicitly justified in ranking-rationale.md
