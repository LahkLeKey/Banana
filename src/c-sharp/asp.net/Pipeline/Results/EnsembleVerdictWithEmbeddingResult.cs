namespace Banana.Api.Pipeline.Results;

/// <summary>
/// Slice 017 -- additive composite verdict that wraps the slice 014
/// <see cref="EnsembleVerdictResult"/> and exposes the Full Brain
/// transformer's 4-dim embedding fingerprint when (and only when) the
/// cascade escalated. On cheap-path verdicts the embedding is absent
/// (null) because the transformer was not invoked.
///
/// JSON shape (FIELD-LOCKED for slice 016 quantization downstream):
///   {
///     "verdict": <slice 014 EnsembleVerdictResult shape>,
///     "embedding": [d0, d1, d2, d3] | null
///   }
///
/// The embedding components are the documented native fingerprint:
/// (banana_context, not_banana_context, attention_delta, banana_probability).
/// </summary>
public sealed record EnsembleVerdictWithEmbeddingResult(
    EnsembleVerdictResult Verdict,
    double[]? Embedding);
