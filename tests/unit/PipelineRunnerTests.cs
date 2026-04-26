using Banana.Api.Pipeline;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class PipelineRunnerTests
{
    private static readonly int[] OrderedTrace = [10, 20, 30];
    private static readonly int[] HaltedTrace = [10, 20];

    [Fact]
    public async Task RunAsync_ExecutesStepsInAscendingOrder()
    {
        var trace = new List<int>();
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new TrackingStep(20, trace),
            new TrackingStep(10, trace),
            new TrackingStep(30, trace),
        };

        var runner = new PipelineRunner<PipelineContext>(steps);
        var result = await runner.RunAsync(new PipelineContext());

        Assert.True(result.IsSuccess);
        Assert.Equal(OrderedTrace, trace);
    }

    [Fact]
    public async Task RunAsync_StopsWhenStepFails()
    {
        var trace = new List<int>();
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new TrackingStep(10, trace),
            new FailingStep(20, trace),
            new TrackingStep(30, trace),
        };

        var runner = new PipelineRunner<PipelineContext>(steps);
        var result = await runner.RunAsync(new PipelineContext());

        Assert.False(result.IsSuccess);
        Assert.Equal(400, result.Problem?.Status);
        Assert.Equal(HaltedTrace, trace);
    }

    [Fact]
    public async Task RunAsync_WithNoSteps_ReturnsSuccess()
    {
        var runner = new PipelineRunner<PipelineContext>(Array.Empty<IPipelineStep<PipelineContext>>());

        var result = await runner.RunAsync(new PipelineContext());

        Assert.True(result.IsSuccess);
        Assert.Null(result.Problem);
    }

    private sealed class TrackingStep(int order, List<int> trace) : IPipelineStep<PipelineContext>
    {
        public int Order => order;

        public Task<PipelineStepResult> ExecuteAsync(PipelineContext context, CancellationToken ct)
        {
            trace.Add(Order);
            return Task.FromResult(PipelineStepResult.Ok());
        }
    }

    private sealed class FailingStep(int order, List<int> trace) : IPipelineStep<PipelineContext>
    {
        public int Order => order;

        public Task<PipelineStepResult> ExecuteAsync(PipelineContext context, CancellationToken ct)
        {
            trace.Add(Order);
            return Task.FromResult(PipelineStepResult.Fail("stop", 400));
        }
    }
}
