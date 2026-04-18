using Banana.Api.NativeInterop;

namespace Banana.Api.Services;

/// <summary>
/// Service result containing the native banana payload plus pipeline metadata for HTTP enrichment.
/// </summary>
/// <param name="BananaResult">Native banana calculation result.</param>
/// <param name="Metadata">Pipeline metadata collected during request execution.</param>
public sealed record BananaCalculationResult(BananaResult BananaResult, IReadOnlyDictionary<string, object> Metadata);