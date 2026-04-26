# Terminology Rules: ML Brain Domain SPIKE

**Date**: 2026-04-26  
**Purpose**: Canonical terminology definitions to be applied consistently across all SPIKE artifacts.

## Canonical Terms

### Brain Domain Terms

| Term | Definition | Do Not Confuse With |
|---|---|---|
| **Left Brain** | The continuous estimation and calibration domain. Handles magnitude reasoning, scoring, and regression-style outputs. | "Left hemisphere" (anatomical); this is a functional domain label only. |
| **Right Brain** | The categorical decisioning and threshold domain. Handles discrete classification, yes/no boundaries, and binary outputs. | "Right hemisphere" (anatomical); this is a functional domain label only. |
| **Full Brain** | The contextual synthesis and sequence interpretation domain. Handles higher-order reasoning across multi-token or multi-feature contexts via transformer-style models. | "Whole brain"; Full Brain is specifically the transformer domain role, not a superset of left and right. |
| **Brain Domain** | One of the three functional ML domains (Left, Right, Full) assigned to Banana's ML capability areas. | Generic ML "domain" usage. |

### Model Family Terms

| Term | Definition | Primary Domain |
|---|---|---|
| **Regression** | A model family producing continuous numeric outputs (e.g., ripeness score 0.0–1.0). Produces calibrated scalar predictions. | Left Brain |
| **Binary** | A model family producing discrete categorical decisions (e.g., is-banana: true/false, class label). Produces threshold-bounded outputs. | Right Brain |
| **Transformer** | A model family using attention-based sequence processing for contextual understanding (e.g., embedding-driven classification or synthesis). | Full Brain |

### Structural Terms

| Term | Definition |
|---|---|
| **Primary Domain** | The canonical domain that owns a model family's primary inference responsibility. Each family has exactly one primary domain. |
| **Secondary Collaboration Edge** | A documented case where a model family provides supporting output to a non-primary domain. Not a co-ownership; the primary remains canonical. |
| **Domain Boundary** | The specification of what inputs, outputs, and decision classes belong exclusively to one brain domain. |
| **Readiness Packet** | A structured follow-up planning artifact: one per domain, contains objective, scope boundary, validation lane expectations, and risk notes. |
| **SPIKE** | A time-boxed discovery artifact. No production behavior change. Produces planning outputs only. |

## Naming Conventions

- Brain domain labels always capitalize first letter: **Left Brain**, **Right Brain**, **Full Brain**.
- Model families always capitalize first letter: **Regression**, **Binary**, **Transformer**.
- Avoid abbreviation "LB", "RB", "FB" in formal artifact titles; acceptable only in table cells with column context.
- Use "primary" not "main" or "core" when describing domain ownership.
- Use "follow-up slice" not "next sprint" when referring to post-SPIKE implementation candidates.

## Prohibited Terms

| Avoid | Preferred |
|---|---|
| "Left hemisphere" / "Right hemisphere" | Left Brain / Right Brain |
| "LLM" when referring to transformer-family specifically | Transformer (model family) |
| "Classification model" ambiguously | Binary (if categorical) or Transformer (if contextual) |
| "Dominant domain" | Primary domain |
