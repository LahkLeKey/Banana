# Research: ML Brain Composition & Productionization SPIKE

**Branch**: `013-ml-brain-composition-spike` | **Phase**: 0 | **Date**: 2026-04-26

## R-01: Composition strategy across Left / Right / Full brains

**Question**: How should the three runnable brains be composed when a single
verdict is required by a downstream consumer (ASP.NET pipeline or UI)?

**Candidates**:
- (a) **Weighted soft vote** -- weighted average of `banana_score` from
  regression, binary, and transformer; threshold at 0.5.
- (b) **Transformer-as-arbiter** -- use Left + Right majority unless they
  disagree, then defer to Full Brain.
- (c) **Gated cascade** -- Right Brain (binary) decides first; only escalate
  to Full Brain when Right Brain confidence is below a band; Left Brain
  supplies the magnitude / probability for downstream calibration.

**Recommendation**: **(c) Gated cascade**.

**Rationale**:
- Matches anchor evidence from 011/012: the binary classifier is permissive
  on the "yellow plastic toy banana" decoy (label=banana) while the
  transformer correctly rejects it (label=not_banana, score 0.44). A cascade
  that escalates uncertain Right Brain verdicts to the Full Brain captures
  the transformer's discriminating power without paying its diagnostics cost
  on every call.
- Aligns with the FB-R03 "default-off cost" property locked in 012: the
  transformer's `_ex` ABI is only invoked on the escalation tier.
- Left Brain's continuous score is the natural calibration signal for any
  UI confidence indicator -- assigning it the magnitude role keeps each
  brain in its strongest semantic role from the 009 SPIKE.

**Tie-break rules (anchored to 010/011/012 calibration)**:
- Right Brain confidence band: `[0.35, 0.65]` of `banana_score` triggers
  escalation to Full Brain. Outside this band Right Brain wins outright.
- On Full Brain escalation, Full Brain verdict is final; Left Brain's
  regression score is reported alongside as the calibration magnitude.
- If any brain returns a non-OK status, ensemble degrades to whichever
  remaining brain has the highest documented anchor sharpness on the
  matching payload class (defaults to Right Brain when nothing else applies).

## R-02: Consumer pull-through mapping

**Question**: Which downstream surfaces should consume the new ABI first?

**Decision**: Sequence consumers as **(1) ASP.NET pipeline step**,
**(2) React badge component**, **(3) Electron preload bridge**.

**Rationale**:
- (1) The pipeline owns the canonical verdict surface and is the only place
  the ensemble strategy from R-01 should live (avoid scattering routing
  logic across managed and JS layers).
- (2) The React UI is the cheapest demonstration of value (ensemble verdict
  badge + optional attention chip behind a feature flag).
- (3) Electron preload bridge only matters once a desktop-only diagnostics
  affordance is requested; defer until a UI signal exists for it.

**Mapping (concrete files / slices)**:

| Brain ABI                                              | Consumer slice                                          | Owner agent              |
|--------------------------------------------------------|---------------------------------------------------------|--------------------------|
| `banana_classify_banana_regression`                    | ASP.NET pipeline calibration step                       | api-pipeline-agent       |
| `banana_classify_banana_binary_with_threshold`         | ASP.NET pipeline gating step (R-01 cascade entry)       | api-pipeline-agent       |
| `banana_classify_banana_transformer_ex`                | ASP.NET pipeline escalation step + optional embedding   | api-interop-agent        |
| Composite verdict JSON                                 | React `BananaBadge` ensemble variant                    | react-ui-agent           |
| Optional attention weights                             | Electron preload bridge (deferred)                      | electron-agent (later)   |

## R-03: Deferred follow-up registry refresh

**Open items at start of 013**:
- **U-001 -- Quantized embedding** (deferred in 012). Needs a real consumer
  before quantization is justified.
- **(009-era)** Cross-family secondary-collaboration edges (documented but
  never actioned).

**Dispositions**:
- U-001 -> **defer-with-trigger**. Trigger: "first cross-process consumer of
  the embedding lands" (e.g. ASP.NET serializing the embedding into the
  pipeline response payload). Until then, double-precision is sufficient.
- 009 secondary edges -> **drop** for now. The cascade strategy from R-01
  obsoletes the secondary-edge framing because composition is centralized
  rather than pairwise.

## R-04: Validation lane for next implementation slices

**Question**: Which lanes must each downstream slice run before close-out?

**Decision**:
- Native lane (`ctest --preset default`) remains the minimum bar for any
  slice that touches `src/native/**`. Composition slices that live in
  ASP.NET MUST additionally run `dotnet test` on the API project plus the
  native lane to prove no contract drift.
- React/Electron consumer slices MUST run their respective bun test
  scripts; they MUST NOT require new native ABI exports unless explicitly
  scoped (additive-only).

**Rationale**: Keeps the additive-ABI discipline (011/012) intact and
prevents accidental cross-layer churn from low-value UI experiments.

## R-05: Sequencing recommendation

**Recommended next-slice order**:

1. **013-followup-A** -- ASP.NET ensemble pipeline step (cascade strategy
   from R-01, single managed slice, no new native exports).
2. **013-followup-B** -- React `BananaBadge` ensemble variant consuming
   the new managed verdict shape.
3. **013-followup-C** -- Quantized embedding native ABI (only after
   013-followup-A serializes the embedding into the response, matching
   U-001's defer-with-trigger condition).

**Rationale**: Maximizes value delivered per slice (each one closes a
visible loop end-to-end), keeps native ABI churn low, and respects the
trigger condition for U-001.
