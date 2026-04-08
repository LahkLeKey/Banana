using CInteropSharp.Api.NativeInterop;
using CInteropSharp.Api.Pipeline;
using CInteropSharp.Api.Services;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class PointsServiceTests
{
    [Fact]
    public void Calculate_DelegatesToPipelineAndReturnsNativeResult()
    {
        var expected = new PointsResult(10, 2, 150, "ok");
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new MockNativeCallStep(expected)
        };
        var executor = new PipelineExecutor<PipelineContext>(steps);
        var service = new PointsService(executor);

        var result = service.Calculate(10, 2);

        Assert.Equal(150, result.Points);
        Assert.Equal("ok", result.Message);
    }

    [Fact]
    public void Calculate_WithNegativeValues_ThrowsClientInputException()
    {
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new ValidationPipelineStep(),
            new MockNativeCallStep(new PointsResult(0, 0, 0, "unused"))
        };
        var executor = new PipelineExecutor<PipelineContext>(steps);
        var service = new PointsService(executor);

        Assert.Throws<ClientInputException>(() => service.Calculate(-1, 2));
    }

    [Fact]
    public void Calculate_WhenNativeStepIsMissing_ThrowsNativeInteropException()
    {
        var executor = new PipelineExecutor<PipelineContext>(Array.Empty<IPipelineStep<PipelineContext>>());
        var service = new PointsService(executor);

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
        private readonly PointsResult _result;

        public MockNativeCallStep(PointsResult result)
        {
            _result = result;
        }

        public int Order => 200;

        public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
        {
            input.NativeResult = _result.Points;
            input.NativePointsResult = _result;
            return next(input);
        }
    }
}
