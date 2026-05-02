using Banana.Api.NativeInterop;

namespace Banana.Api.Pipeline;

/// <summary>
/// Typed PipelineContext (spec 007). Adding a property is non-breaking;
/// removing one requires a step audit.
/// </summary>
public sealed class PipelineContext
{
    public string? Route { get; set; }
    public string? InputJson { get; set; }
    public DateTime StartedAt { get; } = DateTime.UtcNow;
    public NativeStatusCode? LastStatus { get; set; }
    public Dictionary<string, string> Diagnostics { get; } = new(StringComparer.Ordinal);

    /// <summary>
    /// Mutable working state for the ensemble cascade (slice 014).
    /// Populated by EnsembleGatingStep, EnsembleEscalationStep, and
    /// EnsembleCalibrationStep in order. Null on routes that do not use
    /// the ensemble pipeline.
    /// </summary>
    public EnsembleWorkingVerdict? Ensemble { get; set; }

    /// <summary>Spec 080 — A/B variant tag set by AbVariantStep. "A" or "B".</summary>
    public string? AbVariant { get; set; }

    /// <summary>Spec 078/080 — request correlation ID for sampling and variant assignment.</summary>
    public string? CorrelationId { get; set; }
}

/// <summary>
/// Internal working state for the ensemble cascade. Mutated by the three
/// ensemble steps; converted to the immutable EnsembleVerdictResult by the
/// controller before returning to the caller.
/// </summary>
public sealed class EnsembleWorkingVerdict
{
    public string Label { get; set; } = "unknown";
    public double Score { get; set; }
    public bool VerdictLocked { get; set; }
    public bool DidEscalate { get; set; }
    public double CalibrationMagnitude { get; set; }
    public bool Degraded { get; set; }

    /// <summary>
    /// Slice 017 -- captured 4-dim transformer embedding fingerprint when
    /// the route is <c>/ml/ensemble/embedding</c> AND the cascade
    /// escalated. Null in all other cases. Order matches the native
    /// fingerprint: (banana_context, not_banana_context, attention_delta,
    /// banana_probability).
    /// </summary>
    public double[]? CapturedEmbedding { get; set; }
}
