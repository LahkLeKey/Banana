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
    }
}
