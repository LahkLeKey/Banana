using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Steps;

using Xunit;

namespace Banana.UnitTests;

public sealed class PostProcessingStepTests
{
    [Fact]
    public void Order_IsExpected()
    {
        var step = new PostProcessingStep();
        Assert.Equal(300, step.Order);
    }

    [Fact]
    public void Execute_AppliesBonusAtThreshold()
    {
        var step = new PostProcessingStep();
        var context = new PipelineContext
        {
            Purchases = 10,
            Multiplier = 4
        };

        var output = step.Execute(context, static input => input);

        Assert.Equal(20, output.Metadata["bonus.banana"]);
        Assert.Equal(true, output.Metadata["bonus.applied"]);
    }

    [Fact]
    public void Execute_UsesZeroBonusBelowThreshold()
    {
        var step = new PostProcessingStep();
        var context = new PipelineContext
        {
            Purchases = 9,
            Multiplier = 4
        };

        var output = step.Execute(context, static input => input);

        Assert.Equal(0, output.Metadata["bonus.banana"]);
        Assert.Equal(false, output.Metadata["bonus.applied"]);
    }
}
