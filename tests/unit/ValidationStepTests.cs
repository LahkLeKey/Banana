using CInteropSharp.Api.Pipeline;
using CInteropSharp.Api.Pipeline.Steps;
using CInteropSharp.Api.Services;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class ValidationStepTests
{
    [Fact]
    public void Order_IsExpected()
    {
        var step = new ValidationStep();
        Assert.Equal(100, step.Order);
    }

    [Fact]
    public void Execute_ThrowsForNegativeMultiplier()
    {
        var step = new ValidationStep();

        Assert.Throws<ClientInputException>(() => step.Execute(
            new PipelineContext { Purchases = 1, Multiplier = -1 },
            static context => context));
    }

    [Fact]
    public void Execute_ThrowsForNegativePurchases()
    {
        var step = new ValidationStep();

        Assert.Throws<ClientInputException>(() => step.Execute(
            new PipelineContext { Purchases = -1, Multiplier = 1 },
            static context => context));
    }

    [Fact]
    public void Execute_ForValidInput_CallsNext()
    {
        var step = new ValidationStep();
        var called = false;

        var result = step.Execute(
            new PipelineContext { Purchases = 1, Multiplier = 1 },
            context =>
            {
                called = true;
                return context;
            });

        Assert.True(called);
        Assert.Equal(1, result.Purchases);
    }
}
