namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed banana transformer classification response.
/// </summary>
public sealed record BananaMlTransformerClassification(
    string PredictedLabel,
    double BananaProbability,
    double NotBananaProbability,
    double AttentionFocus);
