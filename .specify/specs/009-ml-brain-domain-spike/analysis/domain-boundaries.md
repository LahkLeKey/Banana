# Domain Boundaries: ML Brain Domain SPIKE

**Date**: 2026-04-26  
**Purpose**: Define what belongs exclusively to each brain domain and document edge cases and out-of-domain rules.

## Left Brain Boundaries

### Owned by Left Brain

- Ripeness scoring (continuous 0.0–1.0).
- Magnitude estimation tasks (e.g., size, mass, or nutritional attribute estimation).
- Calibrated probability outputs where ordinal order is meaningful.
- Any model whose loss function is MSE, MAE, or similar continuous regression objective.

### Not owned by Left Brain

- Discrete classification decisions (go to Right Brain).
- Context aggregation across multiple signals or time steps (go to Full Brain).
- Embedding generation (go to Full Brain).

### Edge Case: Softmax output treated as continuous

A model producing a class probability via softmax may appear continuous but is categorical in intent. If the consuming code interprets it as a class decision (threshold applied), it belongs to Right Brain. If it is consumed as a calibrated score without thresholding, it belongs to Left Brain.

**Ruling**: Ownership is determined by consumer intent, not output type alone.

---

## Right Brain Boundaries

### Owned by Right Brain

- Binary is-banana classification (true/false).
- Ripe/unripe/unknown class labeling.
- Any model whose output is a discrete decision from a fixed class set.
- Threshold-boundary enforcement on upstream scores.

### Not owned by Right Brain

- Continuous magnitude estimation (go to Left Brain).
- Multi-signal contextual reasoning (go to Full Brain).
- Models whose primary consumer interprets outputs as ordinal scores.

### Edge Case: Multi-class extends binary

A model with three or more classes (e.g., unripe / ripe / overripe) is Right Brain if its outputs are discrete and threshold-gated, regardless of the number of classes.

**Ruling**: Right Brain owns categorical decisioning for any fixed class count, not only binary (two-class) problems.

---

## Full Brain Boundaries

### Owned by Full Brain

- Attention-based contextual synthesis across multi-feature, multi-modal, or sequential inputs.
- Embedding generation for downstream retrieval or similarity tasks.
- Tasks requiring cross-token or cross-frame reasoning.
- Models that combine Left Brain and Right Brain outputs as input context.

### Not owned by Full Brain

- Simple scalar regression with no contextual aggregation (go to Left Brain).
- Fixed-class classification with no multi-signal context (go to Right Brain).
- Preprocessing pipelines that do not produce inference outputs.

### Edge Case: Transformer used for binary classification

A transformer architecture fine-tuned for binary classification may appear to belong to Right Brain. If its attention mechanism is used, it belongs to Full Brain even if the output is binary, because the mechanism is contextual synthesis.

**Ruling**: Architecture-first for Full Brain. If transformer attention is exercised, the model belongs to Full Brain even if output type resembles Right Brain.

---

## Overlap and Conflict Resolution Rules

| Scenario | Ruling |
|---|---|
| Use case fits both Left and Right Brain | Apply consumer intent rule: if downstream code thresholds the output → Right Brain; if it uses ordinal rank → Left Brain. |
| Transformer recommendation conflicts with simpler model | Prefer simpler model for bounded, well-defined tasks. Transformer is Full Brain only when contextual synthesis adds value the simpler model cannot provide. |
| Model family lacks sufficient evidence for confident domain recommendation | Mark as `[planned]` per confidence-policy.md P-02. Record in uncertainty log. Include research objective in readiness packet rather than implementation scope. |
