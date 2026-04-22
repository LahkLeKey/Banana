using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Steps;

using Xunit;

namespace Banana.UnitTests;

public sealed class NativeCalculationStepTests
{
    [Fact]
    public void Order_IsExpected()
    {
        var step = new NativeCalculationStep(new FakeNativeBananaClient());
        Assert.Equal(200, step.Order);
    }

    [Fact]
    public void Execute_StoresNativeResultOnContext()
    {
        var step = new NativeCalculationStep(new FakeNativeBananaClient());
        var input = new PipelineContext { Purchases = 3, Multiplier = 7 };

        var result = step.Execute(input, static context => context);

        Assert.Equal(21, result.NativeResult);
        Assert.NotNull(result.NativeBananaResult);
        Assert.Equal("purchases=3 multiplier=7 banana=21", result.NativeBananaResult!.Message);
    }

    private sealed class FakeNativeBananaClient : INativeBananaClient
    {
        public BananaResult Calculate(int purchases, int multiplier)
        {
            var banana = purchases * multiplier;
            return new BananaResult(purchases, multiplier, banana, $"purchases={purchases} multiplier={multiplier} banana={banana}");
        }

        public BananaBatchRecord CreateBatch(string batchId, string originFarm, double storageTempC, double ethyleneExposure, int estimatedShelfLifeDays)
        {
            return new BananaBatchRecord(batchId, originFarm, "PACKED", storageTempC, ethyleneExposure, estimatedShelfLifeDays);
        }

        public BananaBatchRecord GetBatchStatus(string batchId)
        {
            return new BananaBatchRecord(batchId, "farm-1", "PACKED", 13.2, 2.5, 3);
        }

        public BananaHarvestBatchRecord CreateHarvestBatch(string harvestBatchId, string fieldId, int harvestDayOrdinal)
        {
            return new BananaHarvestBatchRecord(harvestBatchId, fieldId, harvestDayOrdinal, 0, 0.0);
        }

        public BananaHarvestBatchRecord AddBunchToHarvestBatch(string harvestBatchId, string bunchId, int harvestDayOrdinal, double bunchWeightKg)
        {
            return new BananaHarvestBatchRecord(harvestBatchId, "field-1", harvestDayOrdinal, 1, bunchWeightKg);
        }

        public BananaHarvestBatchRecord GetHarvestBatchStatus(string harvestBatchId)
        {
            return new BananaHarvestBatchRecord(harvestBatchId, "field-1", 1, 1, 12.5);
        }

        public BananaTruckRecord RegisterTruck(string truckId, string nodeId, BananaDistributionNodeType nodeType, double latitude, double longitude, double capacityKg)
        {
            return new BananaTruckRecord(truckId, nodeId, nodeType.ToString().ToUpperInvariant(), latitude, longitude, capacityKg, 0.0, 0);
        }

        public BananaTruckRecord LoadTruckContainer(string truckId, string containerId, double containerWeightKg)
        {
            return new BananaTruckRecord(truckId, "warehouse-1", "WAREHOUSE", 9.90, -79.60, 60.0, containerWeightKg, 1);
        }

        public BananaTruckRecord UnloadTruckContainer(string truckId, string containerId, double containerWeightKg)
        {
            return new BananaTruckRecord(truckId, "warehouse-1", "WAREHOUSE", 9.90, -79.60, 60.0, 0.0, 0);
        }

        public BananaTruckRecord RelocateTruck(string truckId, string nodeId, BananaDistributionNodeType nodeType, double latitude, double longitude)
        {
            return new BananaTruckRecord(truckId, nodeId, nodeType.ToString().ToUpperInvariant(), latitude, longitude, 60.0, 0.0, 0);
        }

        public BananaTruckRecord GetTruckStatus(string truckId)
        {
            return new BananaTruckRecord(truckId, "warehouse-1", "WAREHOUSE", 9.90, -79.60, 60.0, 0.0, 0);
        }

        public BananaRipenessPrediction PredictRipeness(
            IReadOnlyList<double> temperatureHistoryC,
            int daysSinceHarvest,
            double ethyleneExposure,
            double mechanicalDamage,
            double storageTempC)
        {
            return new BananaRipenessPrediction("GREEN", 96, 40.0, 0.1, 0.05);
        }

        public BananaRipenessPrediction PredictBatchRipeness(
            string batchId,
            IReadOnlyList<double> temperatureHistoryC,
            int daysSinceHarvest,
            double mechanicalDamage)
        {
            return new BananaRipenessPrediction("GREEN", 96, 40.0, 0.1, 0.05);
        }

        public double PredictBananaRegressionScore(IReadOnlyList<double> features)
        {
            return 0.75;
        }

        public BananaMlBinaryClassification PredictBananaBinaryClassification(IReadOnlyList<double> features)
        {
            return new BananaMlBinaryClassification("BANANA", 0.8, 0.2, 1.0);
        }

        public BananaMlTransformerClassification PredictBananaTransformerClassification(IReadOnlyList<double> tokenFeatures)
        {
            return new BananaMlTransformerClassification("BANANA", 0.9, 0.1, 0.6);
        }

        public BananaNotBananaClassification ClassifyNotBananaJunk(
            IReadOnlyList<string> tokens,
            int actorCount,
            int entityCount)
        {
            return new BananaNotBananaClassification(
                "BANANA",
                actorCount,
                entityCount,
                tokens.Count,
                tokens.Count,
                0.85,
                0.15,
                0.1);
        }
    }
}
