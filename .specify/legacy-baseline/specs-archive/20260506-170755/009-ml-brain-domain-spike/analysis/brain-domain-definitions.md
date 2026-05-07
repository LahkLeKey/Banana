# Brain Domain Definitions: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Confidence**: High [H] — fixed by spec FR-001, FR-002, FR-003 and research decision R-01

---

## Left Brain Domain

**Label**: Left Brain
**Model Family Primary**: Regression
**Confidence**: High [H]

### Purpose

The Left Brain domain owns continuous estimation, magnitude calibration, and scoring responsibilities within Banana ML. It answers questions of *how much* rather than *which kind*.

### Characteristic Reasoning Style

- Produces scalar, continuous numeric outputs (e.g., ripeness score 0.0 to 1.0).
- Optimizes for minimizing magnitude error (e.g., MSE, MAE objectives).
- Yields calibrated probability or score ranges that preserve ordinal relationships.
- Operates on feature vectors that encode quantity, intensity, or progression signals.

### Expected Inputs

- Feature vectors encoding quantitative measurements (size, color channel intensities, time-series measurements).
- Contextual scalars that describe a banana's physical state or process stage.

### Expected Outputs

- A scalar value in a defined numeric range.
- Confidence interval (optional, for calibrated regression outputs).
- No discrete class label — ordinal outputs only.

### Decision Intent

Rank, score, or estimate an attribute value. The consumer uses the output to set thresholds or drive downstream branching — the Left Brain does not perform that branching itself.

---

## Right Brain Domain

**Label**: Right Brain
**Model Family Primary**: Binary
**Confidence**: High [H]

### Purpose

The Right Brain domain owns categorical decisioning, threshold boundary enforcement, and discrete classification within Banana ML. It answers questions of *which class* or *is it X*.

### Characteristic Reasoning Style

- Produces discrete class labels or binary decisions (e.g., is-banana: true/false, ripe/unripe).
- Optimizes for precision/recall trade-off on a target operating point.
- Yields class probabilities that map to interpretable confidence in a categorical outcome.
- Operates on feature vectors or regression scores from upstream Left Brain outputs.

### Expected Inputs

- Feature vectors with discriminant signals (color histograms, shape metrics, texture features).
- Optionally: Left Brain regression scores as input features for threshold-based categorization.

### Expected Outputs

- A discrete class label from a fixed set.
- Class probability (confidence score per class).
- No continuous ordinal output — categorical outputs only.

### Decision Intent

Decide, classify, or gate. The consumer receives an unambiguous category decision they can act on without further numerical interpretation.

---

## Full Brain Domain

**Label**: Full Brain
**Model Family Primary**: Transformer
**Confidence**: High [H]

### Purpose

The Full Brain domain owns contextual synthesis, sequence interpretation, and higher-order multi-signal reasoning within Banana ML. It answers questions that require aggregating context across multiple signals, tokens, or time steps.

### Characteristic Reasoning Style

- Produces contextualized representations, embeddings, or synthesized classifications.
- Leverages attention mechanisms to weight relevant input signals.
- Capable of integrating Left Brain scores and Right Brain decisions as part of its input context.
- Optimizes for contextual coherence rather than single-instance precision.

### Expected Inputs

- Multi-feature or multi-modal inputs (e.g., image region embeddings, text descriptions, sensor sequences).
- Optionally: Left Brain scores and Right Brain labels as context tokens.

### Expected Outputs

- Synthesized classification, recommendation, or embedding vector.
- Attention-weighted evidence summary (optional, for explainability).
- Output type varies by task — may be categorical, ordinal, or embedding depending on configured head.

### Decision Intent

Synthesize and contextualize. The consumer receives a higher-order answer that incorporates richer context than either the Left or Right Brain alone can provide.

---

## Domain Summary Table

| Domain | Model Family | Reasoning Style | Output Type | Decision Intent |
|---|---|---|---|---|
| Left Brain | Regression | Continuous estimation | Scalar, ordinal | Rank / score |
| Right Brain | Binary | Categorical decisioning | Class label, probability | Decide / classify |
| Full Brain | Transformer | Contextual synthesis | Embedding, synthesis, classification | Synthesize / contextualize |
