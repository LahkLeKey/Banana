namespace Banana.Api.NativeInterop;

/// <summary>
/// Managed representation of a tracked harvest batch stored by native processing logic.
/// </summary>
public sealed record BananaHarvestBatchRecord(
    string HarvestBatchId,
    string FieldId,
    int HarvestDayOrdinal,
    int BunchCount,
    double TotalWeightKg);