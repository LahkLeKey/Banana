# Model Family Tradeoff Matrix: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Dimensions source**: [comparison-dimensions.md](comparison-dimensions.md)
**Scoring convention**: H (High), M (Medium), L (Low). Explainability Burden and Validation Complexity are cost dimensions (lower = better).
**Confidence annotations**: [H] High, [M] Medium per confidence-policy.md.

---

## Matrix

| Dimension | Regression (Left Brain) | Binary (Right Brain) | Transformer (Full Brain) |
|---|---|---|---|
| **Output Determinism** | H [H] | H [H] | M [M] |
| **Calibration Fidelity** | H [H] | M [H] | M [M] |
| **Classification Confidence Clarity** | L [H] | H [H] | M [M] |
| **Contextual Reasoning Depth** | L [H] | L [H] | H [H] |
| **Explainability Burden** | L [H] | L [H] | H [H] |
| **Validation Complexity** | L [H] | M [H] | H [M] |

---

## Cell Rationale Notes

### Output Determinism

- **Regression H [H]**: Given the same input features, a trained regression model always produces the same scalar output. Determinism is intrinsic to the model type.
- **Binary H [H]**: Given the same input features, a trained binary classifier produces the same class decision. Determinism is intrinsic.
- **Transformer M [M]**: Inference with floating-point attention can vary slightly across hardware/runtime environments. Determinism depends on seed control and inference backend. Marked medium and planned-confidence because Banana transformer is not yet implemented.

### Calibration Fidelity

- **Regression H [H]**: Regression outputs are continuous and optimized directly for magnitude fidelity. Calibration is the primary training objective (MSE/MAE).
- **Binary M [H]**: Binary classifiers produce class probabilities that require calibration (Platt scaling or isotonic regression) to align with true event rates. Confidence is high because this is a well-documented limitation.
- **Transformer M [M]**: Calibration depends heavily on fine-tuning approach. Transformers used for classification may under-confident or over-confident without temperature scaling. Marked planned-confidence.

### Classification Confidence Clarity

- **Regression L [H]**: Regression outputs are not designed for classification confidence. A consumer must apply external thresholds, which shifts the decisioning burden out of the model.
- **Binary H [H]**: Binary classifiers produce class probabilities specifically optimized for decision thresholding. Classification confidence clarity is the primary output contract.
- **Transformer M [M]**: Transformer classification heads can produce class probabilities, but attention-based confidence can be less interpretable than binary class confidence. Marked medium.

### Contextual Reasoning Depth

- **Regression L [H]**: Standard regression is single-instance, feature-vector based. No cross-instance or sequential context aggregation.
- **Binary L [H]**: Standard binary classification is single-instance. No contextual aggregation beyond the feature vector.
- **Transformer H [H]**: Attention mechanisms are explicitly designed to aggregate context across tokens, features, or time steps. This is the core architectural strength of transformers.

### Explainability Burden (cost dimension — lower is better)

- **Regression L [H]**: Feature attribution (SHAP, linear weights) is straightforward. Outputs are continuous and interpretable by consumers without specialized tooling.
- **Binary L [H]**: Feature importance and SHAP values are well-established for binary classifiers. Threshold interpretation is intuitive.
- **Transformer H [H]**: Attention maps are non-trivial to interpret. Embedding-level explanations require specialized tools. This is a known and significant explainability cost.

### Validation Complexity (cost dimension — lower is better)

- **Regression L [H]**: Regression tests are threshold-based on MSE/MAE; existing native lane patterns apply directly.
- **Binary M [H]**: Precision/recall curve testing, threshold sweep validation, and negative-sample coverage are required beyond basic assertion tests.
- **Transformer H [M]**: Embedding-level assertions, attention inspection, and distribution shift detection are required. Existing native lane test patterns are insufficient. Marked medium confidence because Banana implementation is pending.

---

## Key Insights

1. Regression and Binary score similarly on determinism and explainability — both are strong for production deployment stability.
2. Transformer is the only family with High contextual reasoning depth — this is the justification for Full Brain's distinct domain role.
3. Transformer has the highest validation complexity — follow-up slice must budget for new test infrastructure, not reuse existing patterns.
4. Binary's medium calibration fidelity is a known production risk — the Right Brain follow-up slice must include calibration work.
