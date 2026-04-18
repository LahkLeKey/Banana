namespace Banana.Api.NativeInterop;

/// <summary>
/// Abstraction for managed-to-native banana calculation calls.
/// </summary>
public interface INativeBananaClient
{
    /// <summary>
    /// Calls native logic and returns managed-friendly results.
    /// </summary>
    /// <param name="purchases">Purchase count.</param>
    /// <param name="multiplier">Multiplier applied by native rules.</param>
    /// <returns>Calculated banana and a human-readable message.</returns>
    BananaResult Calculate(int purchases, int multiplier);

    BananaBatchRecord CreateBatch(
        string batchId,
        string originFarm,
        double storageTempC,
        double ethyleneExposure,
        int estimatedShelfLifeDays);

    BananaBatchRecord GetBatchStatus(string batchId);

    BananaHarvestBatchRecord CreateHarvestBatch(
        string harvestBatchId,
        string fieldId,
        int harvestDayOrdinal);

    BananaHarvestBatchRecord AddBunchToHarvestBatch(
        string harvestBatchId,
        string bunchId,
        int harvestDayOrdinal,
        double bunchWeightKg);

    BananaHarvestBatchRecord GetHarvestBatchStatus(string harvestBatchId);

    BananaTruckRecord RegisterTruck(
        string truckId,
        string nodeId,
        BananaDistributionNodeType nodeType,
        double latitude,
        double longitude,
        double capacityKg);

    BananaTruckRecord LoadTruckContainer(
        string truckId,
        string containerId,
        double containerWeightKg);

    BananaTruckRecord UnloadTruckContainer(
        string truckId,
        string containerId,
        double containerWeightKg);

    BananaTruckRecord RelocateTruck(
        string truckId,
        string nodeId,
        BananaDistributionNodeType nodeType,
        double latitude,
        double longitude);

    BananaTruckRecord GetTruckStatus(string truckId);

    /// <summary>
    /// Calls native lifecycle logic and returns a ripeness prediction.
    /// </summary>
    BananaRipenessPrediction PredictRipeness(
        IReadOnlyList<double> temperatureHistoryC,
        int daysSinceHarvest,
        double ethyleneExposure,
        double mechanicalDamage,
        double storageTempC);

    BananaRipenessPrediction PredictBatchRipeness(
        string batchId,
        IReadOnlyList<double> temperatureHistoryC,
        int daysSinceHarvest,
        double mechanicalDamage);
}
