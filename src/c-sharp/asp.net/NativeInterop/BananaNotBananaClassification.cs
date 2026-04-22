namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed response from the native polymorphic not-banana classifier.
/// </summary>
public sealed record BananaNotBananaClassification(
    string PredictedLabel,
    int ActorCount,
    int EntityCount,
    int SignalTokenCount,
    int TotalTokenCount,
    double BananaProbability,
    double NotBananaProbability,
    double JunkConfidence);
