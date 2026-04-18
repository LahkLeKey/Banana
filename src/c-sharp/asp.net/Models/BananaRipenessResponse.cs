namespace Banana.Api.Models;

/// <summary>
/// Response contract returned by the <c>/ripeness/predict</c> endpoint.
/// </summary>
public sealed record BananaRipenessResponse(
    string BatchId,
    string PredictedStage,
    int ShelfLifeHours,
    double RipeningIndex,
    double SpoilageProbability,
    double ColdChainRisk);