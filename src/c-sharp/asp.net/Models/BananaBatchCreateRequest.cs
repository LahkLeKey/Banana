namespace Banana.Api.Models;

/// <summary>
/// Request contract for creating or updating a tracked banana batch.
/// </summary>
public sealed record BananaBatchCreateRequest(
    string BatchId,
    string OriginFarm,
    double StorageTempC,
    double EthyleneExposure,
    int EstimatedShelfLifeDays);