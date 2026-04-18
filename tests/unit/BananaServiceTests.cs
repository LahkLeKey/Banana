using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Services;

using Xunit;

namespace Banana.UnitTests;

public sealed class BananaServiceTests
{
    [Fact]
    public void Calculate_DelegatesToPipelineAndReturnsNativeResult()
    {
        var expected = new BananaResult(10, 2, 150, "ok");
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new MockNativeCallStep(expected)
        };
        var executor = new PipelineExecutor<PipelineContext>(steps);
        var service = new BananaService(executor);

        var result = service.Calculate(10, 2);

        Assert.Equal(150, result.BananaResult.Banana);
        Assert.Equal("ok", result.BananaResult.Message);
        Assert.Equal("native-dal", result.Metadata["db.source"]);
    }

    [Fact]
    public void Calculate_WithNegativeValues_ThrowsClientInputException()
    {
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new ValidationPipelineStep(),
            new MockNativeCallStep(new BananaResult(0, 0, 0, "unused"))
        };
        var executor = new PipelineExecutor<PipelineContext>(steps);
        var service = new BananaService(executor);

        Assert.Throws<ClientInputException>(() => service.Calculate(-1, 2));
    }

    [Fact]
    public void Calculate_WhenNativeStepIsMissing_ThrowsNativeInteropException()
    {
        var executor = new PipelineExecutor<PipelineContext>(Array.Empty<IPipelineStep<PipelineContext>>());
        var service = new BananaService(executor);

        Assert.Throws<NativeInteropException>(() => service.Calculate(5, 2));
    }

    private sealed class ValidationPipelineStep : IPipelineStep<PipelineContext>
    {
        public int Order => 100;

        public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
        {
            if (input.Purchases < 0 || input.Multiplier < 0)
            {
                throw new ClientInputException("purchases and multiplier must be non-negative.");
            }

            return next(input);
        }
    }

    private sealed class MockNativeCallStep : IPipelineStep<PipelineContext>
    {
        private readonly BananaResult _result;

        public MockNativeCallStep(BananaResult result)
        {
            _result = result;
        }

        public int Order => 200;

        public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
        {
            input.NativeResult = _result.Banana;
            input.NativeBananaResult = _result;
            input.Metadata["db.contract"] = "BananaProfileProjection";
            input.Metadata["db.source"] = "native-dal";
            input.Metadata["db.rowCount"] = 1;
            return next(input);
        }
    }
}
