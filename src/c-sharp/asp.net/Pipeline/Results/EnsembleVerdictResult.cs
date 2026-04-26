namespace Banana.Api.Pipeline.Results;

/// <summary>
/// Composite verdict produced by the ensemble pipeline (slice 014, gated cascade).
///
/// JSON shape (FIELD-LOCKED for downstream React badge consumption -- slice 015):
///   {
///     "label": "banana" | "not_banana" | "unknown",
///     "score": 0.0..1.0,                  // picked-side probability (Right or Full)
///     "did_escalate": true | false,        // true iff Full Brain produced the final verdict
///     "calibration_magnitude": 0.0..1.0,   // Left Brain regression score, never the source of label
///     "status": "ok" | "degraded"          // "degraded" iff one or more brains returned non-OK
///   }
///
/// Field order above is the documented contract for snapshot tests.
/// </summary>
public sealed record EnsembleVerdictResult(
    string Label,
    double Score,
    bool DidEscalate,
    double CalibrationMagnitude,
    string Status);
