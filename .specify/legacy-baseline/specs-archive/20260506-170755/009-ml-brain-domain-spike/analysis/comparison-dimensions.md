# Comparison Dimensions: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Purpose**: Define the normalized shared dimensions used to compare Regression, Binary, and Transformer model families.

## Dimension Set

All three families are scored across the same six dimensions. Each dimension has a definition, scoring scale, and direction-of-good.

---

### Dimension 1: Output Determinism

**Definition**: How predictable and repeatable is the output type given the same inputs?

| Score | Meaning |
|---|---|
| High | Fixed output type with stable numeric range or class set. |
| Medium | Output type is fixed but magnitude/confidence can vary significantly across runs. |
| Low | Output type or structure can shift based on context or model internals. |

**Direction-of-good**: Higher determinism reduces downstream consumer coupling risk.

---

### Dimension 2: Calibration Fidelity

**Definition**: How accurately does the output reflect true underlying probability or magnitude?

| Score | Meaning |
|---|---|
| High | Output reliably maps to ground truth magnitude or probability. |
| Medium | Output is directionally correct but requires recalibration for production use. |
| Low | Output is a rank or relative score that may not correspond to ground truth magnitudes. |

**Direction-of-good**: Higher fidelity reduces calibration overhead in production.

---

### Dimension 3: Classification Confidence Clarity

**Definition**: How cleanly does the model separate confident positive/negative outputs?

| Score | Meaning |
|---|---|
| High | Clear threshold boundaries; high precision/recall separation. |
| Medium | Useful but threshold tuning required for target operating point. |
| Low | Confidence scores or classes overlap significantly; decision boundary is soft. |

**Direction-of-good**: Higher clarity reduces post-model decision logic burden.

---

### Dimension 4: Contextual Reasoning Depth

**Definition**: How much does the model leverage cross-feature or sequential context in its output?

| Score | Meaning |
|---|---|
| High | Output explicitly synthesizes cross-token, multi-feature, or sequential context. |
| Medium | Some cross-feature interaction but primarily single-instance reasoning. |
| Low | Single-feature or narrow-feature inference; no explicit context aggregation. |

**Direction-of-good**: Higher depth supports richer multi-signal inputs but increases compute cost.

---

### Dimension 5: Explainability Burden

**Definition**: How much effort is required to make the model's output interpretable to a non-ML consumer?

| Score | Meaning |
|---|---|
| Low | High explainability — outputs map directly to intuitive explanations. |
| Medium | Moderate explainability — feature attribution tools needed. |
| High | Low explainability — attention maps or embedding analysis required. |

**Direction-of-good**: Lower burden enables faster stakeholder trust and compliance alignment.

---

### Dimension 6: Validation Complexity

**Definition**: How complex are the required tests to certify model behavior in production?

| Score | Meaning |
|---|---|
| Low | Simple threshold-based and regression tests; existing native lane patterns apply. |
| Medium | Requires precision/recall curve testing and calibration checks. |
| High | Requires embedding-level assertions, attention inspection, or distribution shift detection. |

**Direction-of-good**: Lower complexity reduces CI test maintenance overhead.

---

## Scoring Convention

- Use **H** (High), **M** (Medium), **L** (Low) in matrix cells.
- For Explainability Burden and Validation Complexity, lower score is better (these are cost dimensions, not quality dimensions).
- Rationale notes accompany each cell in the tradeoff matrix.

## Dimension Application Rule

Every family-vs-dimension cell in the tradeoff matrix must be completed. No cell may be left blank or marked "TBD" in the final SPIKE artifact.
