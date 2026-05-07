# Confidence and Uncertainty Policy: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Purpose**: Define how confidence, uncertainty, and evidence gaps are handled in all SPIKE analysis artifacts.

## Policy Statements

### P-01: Required Confidence Level per Artifact Type

| Artifact | Required Confidence | Basis |
|---|---|---|
| Domain definitions (US1) | High — must be unambiguous | Derived directly from spec requirements and research decisions |
| Model-family primary mapping (US1) | High — one primary per family | Fixed by spec FR-001 through FR-003 |
| Tradeoff matrix scores (US2) | Medium-High — grounded in known model behavior | Scores sourced from ML model characteristics; not runtime-measured |
| Readiness packet scopes (US3) | Medium — planning-grade estimates | Bounded scope; unknown implementation effort is explicit |

### P-02: Evidence Gap Treatment

When a claim lacks direct Banana runtime evidence (e.g., a transformer model is not yet implemented):

1. Mark the score or claim as `[planned]` in the artifact.
2. Add a rationale note citing the basis for the score (e.g., "known transformer characteristics from R-01").
3. Include in the readiness packet an explicit note: "Validation requires implementation; follow-up lane evidence TBD."

**Do not**: Leave cells blank, use "N/A" without explanation, or assert scores that conflict with known research decisions.

### P-03: Conflicting Evidence Resolution

If two research entries or source artifacts produce conflicting scores:

1. Default to the more conservative score (lower quality dimension, higher cost dimension).
2. Record the conflict explicitly as an uncertainty note in the affected artifact.
3. Include in the readiness packet as a risk item.

### P-04: Uncertainty Escalation Threshold

If uncertainty prevents producing a meaningful primary mapping for a family or meaningful readiness packet for a domain:

1. Record the blocker in [uncertainty-impact-notes.md](uncertainty-impact-notes.md).
2. Scope the readiness packet to the research needed to resolve the uncertainty rather than the implementation.
3. Do not fabricate a mapping to satisfy the artifact template.

### P-05: SPIKE vs Production Confidence Distinction

All confidence levels in this SPIKE are planning-grade, not production-measurement-grade. Confidence here means:

- **High**: Decision is well-bounded by spec, research, and model theory; implementation will not materially change the framing.
- **Medium**: Decision is directionally correct but follow-up validation may refine scores.
- **Low**: Decision is an informed estimate; implementation evidence may revise the framing.

## Confidence Annotation Convention

In artifact tables, append `[H]`, `[M]`, or `[L]` after a score where confidence level is explicitly tracked.

Example: `High [H]` — score is high confidence.
Example: `Medium [M]` — score is medium confidence, may be refined by implementation evidence.

## Uncertainty Log

| ID | Artifact | Uncertainty | Impact | Resolution Path |
|---|---|---|---|---|
| U-001 | model-family-tradeoff-matrix.md | Transformer implementation does not yet exist in Banana; scores based on family characteristics | Validation Complexity and Calibration Fidelity rows are planned-grade | Resolved in follow-up transformer implementation slice |
| U-002 | domain-fit-scorecards.md | Regression and Binary implementations may not yet be fully instrumented | Calibration Fidelity score needs runtime measurement to confirm | Resolved in follow-up per-domain implementation slices |
