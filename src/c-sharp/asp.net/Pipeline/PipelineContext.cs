using Banana.Api.NativeInterop;
using Banana.Api.DataAccess;

namespace Banana.Api.Pipeline;

/// <summary>
/// Mutable state container shared by all pipeline steps in a single request.
/// </summary>
public sealed class PipelineContext
{
    /// <summary>
    /// Gets or sets the purchase count from the incoming request.
    /// </summary>
    public int Purchases { get; set; }

    /// <summary>
    /// Gets or sets the multiplier from the incoming request.
    /// </summary>
    public int Multiplier { get; set; }

    /// <summary>
    /// Gets or sets the numeric banana produced by native calculation.
    /// </summary>
    public int NativeResult { get; set; }

    /// <summary>
    /// Gets or sets the complete native response mapped to managed types.
    /// </summary>
    public BananaResult? NativeBananaResult { get; set; }

    /// <summary>
    /// Gets or sets extensible key/value data used for enrichment, diagnostics, and future steps.
    /// </summary>
    public Dictionary<string, object> Metadata { get; set; } = new();

    /// <summary>
    /// Gets or sets the raw data access result produced by the database pipeline stage.
    /// </summary>
    public RawDbAccessResult? DatabaseRawResult { get; set; }
}
