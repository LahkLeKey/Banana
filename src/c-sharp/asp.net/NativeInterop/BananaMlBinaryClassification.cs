namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed banana binary classification response.
/// </summary>
public sealed record BananaMlBinaryClassification(
    string PredictedLabel,
    double BananaProbability,
    double NotBananaProbability,
    double DecisionMargin);
