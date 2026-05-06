# Deferred follow-up registry refresh (013 SPIKE, R-03 expansion)

## Open items at the start of this SPIKE

### U-001 -- Quantized embedding for transformer

- **Source**: 009 SPIKE recommendations -> deferred again in
  [012-full-brain-transformer-runnable/analysis/residual-followups.md](../../012-full-brain-transformer-runnable/analysis/residual-followups.md).
- **Disposition**: `fulfilled` (was `trigger-fired`).
- **Fulfilled by**: Slice **016** (`016-transformer-quantized-embedding`)
  shipped 2026-04-27. Native ctest 7/7 → 8/8, ABI 2.2 → 2.3 additive only,
  legacy `_ex` JSON byte-identical, reconstruction error bounded by
  `scale/2 + 1e-9` across all 6 SPIKE anchors. See
  [../../016-transformer-quantized-embedding/analysis/abi-additive-evidence.md](../../016-transformer-quantized-embedding/analysis/abi-additive-evidence.md).
- **Trigger condition (historical)**: First cross-process consumer of the embedding
  lands. In practice this means slice **014** has been merged AND a
  controller / pipeline step is serializing the
  `BANANA_ML_TRANSFORMER_EMBEDDING_DIM=4` doubles into the response payload.
- **Trigger fired**: Slice **017**
  (`017-ensemble-verdict-embedding-passthrough`) shipped the additive
  `/ml/ensemble/embedding` route that serializes the 4-dim embedding on
  escalated verdicts via the existing slice-012
  `banana_classify_banana_transformer_ex` export. ABI stayed at 2.2;
  native ctest stayed at 7/7. The cross-process embedding consumer
  contract is now live.
- **Next action when triggered**: Open slice **016** using readiness packet
  [followup-C-quantized-embedding.md](./followup-C-quantized-embedding.md).
- **Owner when triggered**: native-wrapper-agent (additive ABI), with
  api-interop-agent for the managed payload contract.

### 009-era pairwise secondary collaboration edges

- **Source**: 009 SPIKE R-02 ("documented secondary collaboration edges").
- **Disposition**: `drop`.
- **Rationale**: The gated cascade in
  [composition-strategy.md](./composition-strategy.md) centralizes
  composition. Pairwise secondary edges are obsoleted because all
  cross-brain interaction now flows through the single ASP.NET pipeline
  cascade. Re-introducing pairwise edges would duplicate routing logic.

## New items emitted by this SPIKE

None. The SPIKE intentionally does NOT defer any new items; everything
identified is either ready (014, 015) or already gated (016).

## Disposition vocabulary

- **ready-now** -- start the slice immediately; readiness packet exists.
- **defer-with-trigger** -- packet exists; gated by the named trigger.
- **drop** -- removed from the registry; rationale captured here.

## Cross-link

This file is referenced from
[012-full-brain-transformer-runnable/analysis/residual-followups.md](../../012-full-brain-transformer-runnable/analysis/residual-followups.md)
via the SPIKE close-out (T008).
