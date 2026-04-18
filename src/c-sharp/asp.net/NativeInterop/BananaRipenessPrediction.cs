namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed ripeness prediction returned by native lifecycle logic.
/// </summary>
public sealed record BananaRipenessPrediction(
    string PredictedStage,
    int ShelfLifeHours,
    double RipeningIndex,
    double SpoilageProbability,
    double ColdChainRisk);