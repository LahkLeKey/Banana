namespace Banana.Api.Models;

/// <summary>
/// Request contract for native banana ripeness prediction.
/// </summary>
public sealed record BananaRipenessRequest(
    string BatchId,
    IReadOnlyList<double> TemperatureHistory,
    int DaysSinceHarvest,
    double EthyleneExposure,
    double MechanicalDamage,
    double? StorageTempC);