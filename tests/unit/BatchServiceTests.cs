using Banana.Api.Models;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Xunit;

namespace Banana.UnitTests;

public sealed class BatchServiceTests
{
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

        public BananaRipenessPrediction PredictRipeness(IReadOnlyList<double> temperatureHistoryC, int daysSinceHarvest, double ethyleneExposure, double mechanicalDamage, double storageTempC)
        {
            return new BananaRipenessPrediction("YELLOW", 48, 180.0, 0.3, 0.15);
        }

        public BananaRipenessPrediction PredictBatchRipeness(string batchId, IReadOnlyList<double> temperatureHistoryC, int daysSinceHarvest, double mechanicalDamage)
        {
            return new BananaRipenessPrediction("YELLOW", 48, 180.0, 0.3, 0.15);
        }
    }
}