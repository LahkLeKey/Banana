using Banana.Api.Models;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Xunit;

namespace Banana.UnitTests;

public sealed class BatchServiceTests
{
    [Fact]
    public void Create_WithNullRequest_ThrowsClientInputException()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Create(null!));
    }

    [Fact]
    public void Create_WithMissingBatchId_ThrowsClientInputException()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() =>
            service.Create(new BananaBatchCreateRequest("", "farm-1", 13.2, 2.5, 3)));
    }

    [Fact]
    public void Create_WithMissingOriginFarm_ThrowsClientInputException()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() =>
            service.Create(new BananaBatchCreateRequest("batch-1", "", 13.2, 2.5, 3)));
    }

    [Fact]
    public void Create_WithNegativeStorageTemperature_ThrowsClientInputException()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() =>
            service.Create(new BananaBatchCreateRequest("batch-1", "farm-1", -0.1, 2.5, 3)));
    }

    [Fact]
    public void Create_WithNegativeEthyleneExposure_ThrowsClientInputException()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() =>
            service.Create(new BananaBatchCreateRequest("batch-1", "farm-1", 13.2, -0.1, 3)));
    }

    [Fact]
    public void Create_WithNegativeShelfLifeDays_ThrowsClientInputException()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() =>
            service.Create(new BananaBatchCreateRequest("batch-1", "farm-1", 13.2, 2.5, -1)));
    }

    [Fact]
    public void Create_DelegatesToNativeInteropAndReturnsMappedResponse()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        var result = service.Create(new BananaBatchCreateRequest("batch-1", "farm-1", 13.2, 2.5, 3));

        Assert.Equal("batch-1", result.BatchId);
        Assert.Equal("farm-1", result.OriginFarm);
        Assert.Equal("PACKED", result.ExportStatus);
    }

    [Fact]
    public void GetStatus_WithMissingId_ThrowsClientInputException()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.GetStatus(""));
    }

    [Fact]
    public void GetStatus_ReturnsMappedResponse()
    {
        var service = new BatchService(new FakeNativeBananaClient());

        var result = service.GetStatus("batch-1");

        Assert.Equal("batch-1", result.BatchId);
        Assert.Equal("farm-1", result.OriginFarm);
        Assert.Equal("PACKED", result.ExportStatus);
        Assert.Equal(13.2, result.StorageTempC, 3);
        Assert.Equal(2.5, result.EthyleneExposure, 3);
        Assert.Equal(3, result.EstimatedShelfLifeDays);
    }

    private sealed class FakeNativeBananaClient : INativeBananaClient
    {
        public BananaResult Calculate(int purchases, int multiplier)
        {
            return new BananaResult(purchases, multiplier, purchases * multiplier, "unused");
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

        public BananaRipenessPrediction PredictRipeness(IReadOnlyList<double> temperatureHistoryC, int daysSinceHarvest, double ethyleneExposure, double mechanicalDamage, double storageTempC)
        {
            return new BananaRipenessPrediction("YELLOW", 48, 180.0, 0.3, 0.15);
        }

        public BananaRipenessPrediction PredictBatchRipeness(string batchId, IReadOnlyList<double> temperatureHistoryC, int daysSinceHarvest, double mechanicalDamage)
        {
            return new BananaRipenessPrediction("YELLOW", 48, 180.0, 0.3, 0.15);
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