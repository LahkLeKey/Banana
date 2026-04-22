namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed banana binary classification response.
/// </summary>
public sealed record BananaMlBinaryClassification(
    string PredictedLabel,
    double BananaProbability,
    double NotBananaProbability,
    double DecisionMargin)
{
    public double JaccardSimilarity { get; init; }

    public BananaMlBinaryConfusionMatrix ConfusionMatrix { get; init; } =
        new(0.0, 0.0, 0.0, 0.0);
}
