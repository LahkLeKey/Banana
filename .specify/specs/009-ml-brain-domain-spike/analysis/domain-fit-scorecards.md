# Domain Fit Scorecards: ML Brain Domain SPIKE

**Date**: 2026-04-26  
**Purpose**: Score and rank each model family against each brain domain to confirm primary mapping alignment.

## Scoring Method

For each family-vs-domain pair, score the fit using a weighted assessment of the six comparison dimensions:

- **Quality dimensions** (higher is better): Output Determinism, Calibration Fidelity, Classification Confidence Clarity, Contextual Reasoning Depth
- **Cost dimensions** (lower is better): Explainability Burden, Validation Complexity

Score scale:
- 3 = Strong fit (H on quality, L on cost)
- 2 = Moderate fit (M on either)
- 1 = Weak fit (L on quality, H on cost)

---

## Scorecard: Left Brain Domain (Estimation Mandate)

The Left Brain domain requires: high output determinism, high calibration fidelity, low explainability burden, low validation complexity. Contextual reasoning depth is not a priority.

| Model Family | Output Determinism | Calibration Fidelity | Classif. Confidence Clarity | Contextual Reasoning | Explainability Burden | Validation Complexity | **Total** | **Fit** |
|---|---|---|---|---|---|---|---|---|
| **Regression** | 3 | 3 | 1 (not needed) | 1 (not needed) | 3 | 3 | **14** | ✅ Primary |
| Binary | 3 | 2 | 3 | 1 | 3 | 2 | 14 | — |
| Transformer | 2 | 2 | 2 | 3 | 1 | 1 | 11 | ✗ Poor fit |

**Primary: Regression.** Regression uniquely maximizes calibration fidelity — the defining Left Brain requirement. Binary ties on total but is semantically inappropriate for continuous magnitude output. Tie-break: calibration fidelity is the mandated property; Regression wins.

---

## Scorecard: Right Brain Domain (Decisioning Mandate)

The Right Brain domain requires: high classification confidence clarity, high output determinism, moderate calibration fidelity. Contextual reasoning depth is not a priority.

| Model Family | Output Determinism | Calibration Fidelity | Classif. Confidence Clarity | Contextual Reasoning | Explainability Burden | Validation Complexity | **Total** | **Fit** |
|---|---|---|---|---|---|---|---|---|
| Regression | 3 | 3 | 1 | 1 | 3 | 3 | 14 | — |
| **Binary** | 3 | 2 | 3 | 1 | 3 | 2 | **14** | ✅ Primary |
| Transformer | 2 | 2 | 2 | 3 | 1 | 1 | 11 | ✗ Poor fit |

**Primary: Binary.** Binary uniquely maximizes Classification Confidence Clarity — the defining Right Brain requirement. Regression ties on total but produces ordinal outputs rather than categorical decisions. Tie-break: Classification Confidence Clarity is the mandated property; Binary wins.

---

## Scorecard: Full Brain Domain (Synthesis Mandate)

The Full Brain domain requires: high contextual reasoning depth. Determinism, calibration, and classification clarity are secondary. Explainability burden and validation complexity are accepted as high costs for the synthesis capability.

| Model Family | Output Determinism | Calibration Fidelity | Classif. Confidence Clarity | Contextual Reasoning | Explainability Burden | Validation Complexity | **Total** | **Fit** |
|---|---|---|---|---|---|---|---|---|
| Regression | 3 | 3 | 1 | 1 | 3 | 3 | 14 | ✗ Poor fit |
| Binary | 3 | 2 | 3 | 1 | 3 | 2 | 14 | ✗ Poor fit |
| **Transformer** | 2 | 2 | 2 | 3 | 1 | 1 | **11** | ✅ Primary |

**Primary: Transformer.** Transformer uniquely achieves High contextual reasoning depth — the defining Full Brain requirement. The lower total is not a disqualifier; Full Brain explicitly accepts higher cost for synthesis capability that Regression and Binary cannot provide.

---

## Summary

| Brain Domain | Primary Family | Confidence | Defining Dimension |
|---|---|---|---|
| Left Brain | Regression | High [H] | Calibration Fidelity |
| Right Brain | Binary | High [H] | Classification Confidence Clarity |
| Full Brain | Transformer | High [H] | Contextual Reasoning Depth |
