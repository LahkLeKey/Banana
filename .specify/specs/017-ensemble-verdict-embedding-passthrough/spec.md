# Feature Specification: Ensemble Verdict Embedding Passthrough

**Feature Branch**: `[017-ensemble-verdict-embedding-passthrough]`
**Created**: 2026-04-26
**Status**: Draft -- ready to execute
**Depends on**: 014 ASP.NET Ensemble Pipeline (merged)
**Unlocks**: 016 Transformer Quantized Embedding (trigger condition)
**SPIKE source**: New incremental slice. No new SPIKE required; the native
`banana_classify_banana_transformer_ex` export already carries the 4-dim
embedding (added in slice 012). This slice only plumbs it through the
managed ASP.NET surface as an opt-in cross-process consumer.

## In Scope

- Add a managed P/Invoke binding for the existing native
  `banana_classify_banana_transformer_ex` export (with
  `log_attention=0`, embedding requested) to `INativeBananaClient`.
- Add a new ASP.NET route `POST /ml/ensemble/embedding` that runs the
  014 gated cascade and additionally returns the 4-dim transformer
  embedding when the cascade escalates.
- Add a new payload `EnsembleVerdictWithEmbeddingResult` that wraps the
  existing `EnsembleVerdictResult` and adds an `embedding: double[4]`
  field. The legacy `EnsembleVerdictResult` shape is unchanged.

## Out of Scope

- Changes to `src/native/**`. No new exports, no ABI bump.
- Changes to the existing `POST /ml/ensemble` route or
  `EnsembleVerdictResult` JSON shape (014 contract preserved).
- React/Electron consumption of the embedding.
- Quantization (that is slice 016, which this slice unlocks).

## User Scenarios

### US1 -- Passthrough on escalation (P1)
When the cascade escalates to the Full Brain, the response carries the
transformer's 4-component embedding alongside the standard verdict.

**Acceptance**: For each of the 3 SPIKE escalation anchors (decoy,
ambiguous-yellow, empty-text), `embedding.length == 4` and every
component is finite.

### US2 -- Cheap path returns no embedding (P2)
When the cascade locks at the gating step (out-of-band binary score),
the response carries `embedding: null`. The transformer is NOT invoked.

**Acceptance**: For each of the 3 SPIKE cheap-path anchors,
`embedding == null` and the transformer call count is zero.

### US3 -- Legacy route untouched (P1)
The `POST /ml/ensemble` route's JSON shape is unchanged. A snapshot test
locks the 5-field contract from slice 014.

## Requirements

- **P-R01**: New managed binding MUST call the existing
  `banana_classify_banana_transformer_ex` export with `log_attention=0`
  and a 4-double embedding buffer. No new native exports.
- **P-R02**: Native lane MUST stay 7/7. ABI version unchanged at 2.2.
- **P-R03**: Legacy `POST /ml/ensemble` route response shape MUST be
  byte-identical to the slice 014 contract.
- **P-R04**: New `EnsembleVerdictWithEmbeddingResult` JSON shape locked
  to `{ verdict: <014 shape>, embedding: double[4] | null }` with field
  order documented in XML doc comments (so slice 016's quantization
  test can rely on the cross-process serialization).
- **P-R05**: When the gating step locks the verdict (no escalation),
  the embedding MUST be `null` (no transformer invocation).

## Success Criteria

- ctest 7/7 unchanged.
- `dotnet test` green; no regression in 014 tests.
- New tests cover all 6 SPIKE anchor walks (3 cheap, 3 escalated).
- Slice 016's gate trigger fires on this slice's merge.

## Validation lane

```
cmake --preset default && cmake --build --preset default
ctest --preset default --output-on-failure   # expect 7/7 unchanged
dotnet test src/c-sharp/asp.net/asp.net.sln --collect:"XPlat Code Coverage"
bash scripts/check-dotnet-coverage-threshold.sh
```

## In-scope files

See [analysis/in-scope-files.md](./analysis/in-scope-files.md).
