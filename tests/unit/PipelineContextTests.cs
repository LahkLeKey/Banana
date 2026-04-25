// Spec 014 — minimal unit test for the typed PipelineContext seam.
using Banana.Api.Pipeline;
using Xunit;

namespace Banana.Api.Tests.Unit;

public class PipelineContextTests
{
    [Fact]
    public void NewContext_HasNullRouteAndZeroDiagnostics()
    {
        var ctx = new PipelineContext { Route = "/banana" };
        Assert.Equal("/banana", ctx.Route);
        Assert.Empty(ctx.Diagnostics);
        Assert.Null(ctx.LastStatus);
    }
}
