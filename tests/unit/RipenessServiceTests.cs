using Banana.Api.Models;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Xunit;

namespace Banana.UnitTests;

public sealed class RipenessServiceTests
{
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