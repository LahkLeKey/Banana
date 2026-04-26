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
}
