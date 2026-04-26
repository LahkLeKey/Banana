# Residual follow-ups -- 012 Full Brain Transformer

## U-001 -- Quantized embedding (deferred from 009 SPIKE)
The current Full Brain transformer emits a 4-component double-precision
fingerprint embedding via `banana_classify_banana_transformer_ex`. The 009
SPIKE recommended exploring a quantized (int8) representation to reduce
downstream payload size when shipping the embedding across the
ASP.NET/Electron boundary.

Status: **DEFERRED**. The double-precision fingerprint is sufficient for
US1-US4 acceptance and downstream snapshot tests in this slice. The
quantization decision should be re-opened only after the embedding has a
real consumer (e.g. a similarity-search index or a downstream cross-model
ensemble in slice 013+).

Owner / next action: open a Future-tag issue when the first consumer lands.

**Re-scored by 013 SPIKE (2026-04-26)**:
disposition = `defer-with-trigger`; trigger = "slice 014 (ASP.NET ensemble)
serializes the embedding into a cross-process payload". See
[../../013-ml-brain-composition-spike/analysis/deferred-registry.md](../../013-ml-brain-composition-spike/analysis/deferred-registry.md)
and the gated readiness packet
[../../013-ml-brain-composition-spike/analysis/followup-C-quantized-embedding.md](../../013-ml-brain-composition-spike/analysis/followup-C-quantized-embedding.md).

## (None other open at slice close.)
