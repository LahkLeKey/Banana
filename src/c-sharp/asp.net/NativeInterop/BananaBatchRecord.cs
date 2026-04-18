namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed representation of a tracked banana batch stored by native lifecycle logic.
/// </summary>
public sealed record BananaBatchRecord(
    string BatchId,
    string OriginFarm,
    string ExportStatus,
    double StorageTempC,
    double EthyleneExposure,
    int EstimatedShelfLifeDays);