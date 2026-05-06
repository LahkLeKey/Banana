# Model Family Mappings: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Confidence**: High [H] — fixed by spec FR-001 through FR-003 and research decision R-01, R-02

## Primary Mapping Table

| Model Family | Primary Domain | Confidence | Rationale |
|---|---|---|---|
| Regression | Left Brain | High [H] | Continuous scalar output matches Left Brain's estimation mandate. Regression minimizes magnitude error over a continuous output space — the defining property of the Left Brain domain. |
| Binary | Right Brain | High [H] | Discrete class decision matches Right Brain's categorical mandate. Binary models produce threshold-bounded classification that aligns with the Right Brain's decisioning role. |
| Transformer | Full Brain | High [H] | User directive fixes Transformer as Full Brain. Transformer attention mechanisms match the contextual synthesis and sequence reasoning mandate of the Full Brain domain. |

## Rationale Detail

### Regression → Left Brain

**Primary basis**: Regression models produce continuous numeric outputs — inherently the definition of estimation and calibration tasks. Assigning Regression to any other domain would conflict with the Left Brain's continuous output mandate.

**Alternative rejected**: Left Brain = Binary. Binary models produce categorical decisions, not continuous estimates. This assignment would collapse the domain distinction.

**Source**: research.md R-01, spec FR-001.

---

### Binary → Right Brain

**Primary basis**: Binary classification produces discrete class labels. This is the core categorical decisioning function of the Right Brain. The threshold-boundary nature of binary decisions matches the Right Brain's operational role.

**Alternative rejected**: Right Brain = Regression. Regression outputs lack the categorical decisioning intent of the Right Brain. This assignment would merge Left and Right purposes.

**Source**: research.md R-01, spec FR-002.

---

### Transformer → Full Brain

**Primary basis**: User directive explicitly assigns Transformer to Full Brain. Architecturally, transformers use attention to aggregate and synthesize multi-signal context — a perfect fit for the Full Brain's higher-order reasoning role.

**Alternative rejected**: Full Brain = split across all families. Research decision R-01 rejected this due to ownership ambiguity.

**Source**: user directive (session 2026-04-26), research.md R-01, spec FR-003.

---

## Secondary Collaboration Edges

Collaboration edges are documented here but do not affect primary ownership.

| Source Domain | Target Domain | Collaboration Pattern | Status |
|---|---|---|---|
| Left Brain (Regression) | Right Brain (Binary) | Regression scores used as input features to Binary classifier | Known pattern; does not change primary ownership |
| Left Brain + Right Brain | Full Brain (Transformer) | Scores and labels from Left/Right used as context tokens for transformer input | Planned pattern; does not change primary ownership |

**Rule**: Secondary edges are informational only. They do not make a model family a co-owner of a domain.
