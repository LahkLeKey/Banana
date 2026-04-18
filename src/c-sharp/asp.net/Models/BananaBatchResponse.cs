namespace Banana.Api.Models;

/// <summary>
/// HTTP response contract returned by banana batch management endpoints.
/// </summary>
public sealed record BananaBatchResponse(
    string BatchId,
    string OriginFarm,
    string ExportStatus,
    double StorageTempC,
    double EthyleneExposure,
    int EstimatedShelfLifeDays);