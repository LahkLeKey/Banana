# Uncertainty Impact Notes: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Source**: confidence-policy.md uncertainty log (U-001, U-002)

## Summary

Two uncertainty items affect ranking and scorecard scores. Both are planned-grade entries that will be resolved in follow-up implementation slices.

---

## U-001: Transformer Not Yet Implemented

**Affected artifact**: model-family-tradeoff-matrix.md
**Affected cells**: Transformer — Output Determinism, Calibration Fidelity, Classification Confidence Clarity, Validation Complexity
**Confidence level**: Medium [M]

### Impact on Ranking

- Transformer ranks as Primary for Full Brain regardless of confidence level because no other family provides Contextual Reasoning Depth at H.
- If implementation reveals Transformer determinism is lower than M or validation complexity is higher than H, the Full Brain readiness packet will need a compute-cost note revision.
- **Ranking stability**: High. The Full Brain assignment is mandate-driven, not score-total-driven.

### Resolution Path

Resolved when the Full Brain follow-up implementation slice runs baseline inference experiments and captures runtime determinism and validation complexity evidence.

---

## U-002: Regression and Binary Runtime Calibration Unmeasured

**Affected artifact**: domain-fit-scorecards.md
**Affected cells**: Regression — Calibration Fidelity; Binary — Calibration Fidelity
**Confidence level**: Medium [M] for production calibration; High [H] for theoretical family characteristics

### Impact on Ranking

- Regression's High Calibration Fidelity is grounded in theory (MSE objective) and is likely to hold in practice, but runtime measurement against Banana-specific data may reveal recalibration needs.
- Binary's Medium Calibration Fidelity is a known characteristic (Platt scaling required); this is not a surprise risk — it is a budgeted follow-up action.
- **Ranking stability**: High for Left Brain primary (Regression); the tie-break gap on Calibration Fidelity is large enough that runtime measurement would need to completely invert the H vs M gap to change the outcome.

### Resolution Path

Resolved when Left Brain and Right Brain follow-up implementation slices include calibration validation tests and runtime evidence is captured.

---

## Impact Summary

| Uncertainty | Ranking Impact | Affected Follow-Up |
|---|---|---|
| U-001: Transformer not implemented | Low (Full Brain mandate-driven; not score-total-driven) | Full Brain follow-up slice |
| U-002: Runtime calibration unmeasured | Low (tie-break gap is large) | Left Brain and Right Brain follow-up slices |

**Overall ranking stability**: High. No uncertainty item is likely to change primary mappings.
