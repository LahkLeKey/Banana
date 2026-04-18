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
    }
}
