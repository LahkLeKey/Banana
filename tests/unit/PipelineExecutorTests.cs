using Banana.Api.Pipeline;

using Xunit;

namespace Banana.UnitTests;

public sealed class PipelineExecutorTests
{
    [Fact]
    public void Execute_RunsStepsInAscendingOrder()
    {
        var context = new PipelineContext
        {
            Purchases = 3,
            Multiplier = 2
        };

        var executed = new List<string>();
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new TrackingStep("third", 300, executed),
            new TrackingStep("first", 100, executed),
            new TrackingStep("second", 200, executed)
        };
        var executor = new PipelineExecutor<PipelineContext>(steps);

        executor.Execute(context);

        Assert.Equal(new[] { "first", "second", "third" }, executed);
    }

    [Fact]
    public void Execute_AllowsStepToTransformContext()
    {
        var context = new PipelineContext
        {
            Purchases = 12,
            Multiplier = 2
        };
        var steps = new IPipelineStep<PipelineContext>[]
        {
            new BonusMutationStep()
        };
        var executor = new PipelineExecutor<PipelineContext>(steps);

        var result = executor.Execute(context);

        Assert.Equal(10, result.NativeResult);
    }

    private sealed class TrackingStep : IPipelineStep<PipelineContext>
    {
        private readonly string _name;
        private readonly List<string> _executed;

        public TrackingStep(string name, int order, List<string> executed)
        {
            _name = name;
            _executed = executed;
            Order = order;
        }

        public int Order { get; }

        public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
        {
            _executed.Add(_name);
            return next(input);
        }
    }

    private sealed class BonusMutationStep : IPipelineStep<PipelineContext>
    {
        public int Order => 150;

        public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
        {
            input.NativeResult = input.Purchases >= 10 ? input.Multiplier * 5 : 0;
            return next(input);
        }
    }
}
