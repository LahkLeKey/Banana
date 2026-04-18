using Banana.Api.Models;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Xunit;

namespace Banana.UnitTests;

public sealed class RipenessServiceTests
{
    [Fact]
    public void Predict_WithNullRequest_ThrowsClientInputException()
    {
        var service = new RipenessService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Predict(null!));
    }

    [Fact]
    public void Predict_WithMissingBatchId_ThrowsClientInputException()
    {
        var service = new RipenessService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Predict(
            new BananaRipenessRequest("", [12.5], 5, 1.0, 0.0, null)));
    }

    [Fact]
    public void Predict_DelegatesToNativeInteropAndReturnsMappedResponse()
    {
        var service = new RipenessService(new FakeNativeBananaClient());
        var request = new BananaRipenessRequest("batch-1", [12.5, 13.0, 14.2], 5, 2.5, 0.1, null);

        var result = service.Predict(request);

        Assert.Equal("batch-1", result.BatchId);
        Assert.Equal("YELLOW", result.PredictedStage);
        Assert.Equal(48, result.ShelfLifeHours);
    }

    [Fact]
    public void Predict_WithEmptyHistory_ThrowsClientInputException()
    {
        var service = new RipenessService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Predict(
            new BananaRipenessRequest("batch-1", [], 5, 1.0, 0.0, null)));
    }

    [Fact]
    public void Predict_WithNullHistory_ThrowsClientInputException()
    {
        var service = new RipenessService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Predict(
            new BananaRipenessRequest("batch-1", null!, 5, 1.0, 0.0, null)));
    }

    [Fact]
    public void Predict_WithNegativeDaysSinceHarvest_ThrowsClientInputException()
    {
        var service = new RipenessService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Predict(
            new BananaRipenessRequest("batch-1", [12.5], -1, 1.0, 0.0, null)));
    }

    [Fact]
    public void Predict_WithNegativeEthyleneExposure_ThrowsClientInputException()
    {
        var service = new RipenessService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Predict(
            new BananaRipenessRequest("batch-1", [12.5], 5, -0.1, 0.0, null)));
    }

    [Fact]
    public void Predict_WithNegativeMechanicalDamage_ThrowsClientInputException()
    {
        var service = new RipenessService(new FakeNativeBananaClient());

        Assert.Throws<ClientInputException>(() => service.Predict(
            new BananaRipenessRequest("batch-1", [12.5], 5, 1.0, -0.1, null)));
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

        public BananaRipenessPrediction PredictRipeness(
            IReadOnlyList<double> temperatureHistoryC,
            int daysSinceHarvest,
            double ethyleneExposure,
            double mechanicalDamage,
            double storageTempC)
        {
            return new BananaRipenessPrediction("YELLOW", 48, 180.0, 0.3, 0.15);
        }

        public BananaRipenessPrediction PredictBatchRipeness(
            string batchId,
            IReadOnlyList<double> temperatureHistoryC,
            int daysSinceHarvest,
            double mechanicalDamage)
        {
            return new BananaRipenessPrediction("YELLOW", 48, 180.0, 0.3, 0.15);
        }
    }
}