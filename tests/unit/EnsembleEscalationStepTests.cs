using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Steps;
using Banana.Api.Tests.Unit.TestDoubles;

using Xunit;

namespace Banana.Api.Tests.Unit;

/// <summary>
/// Slice 014 -- US2 (escalation) coverage. The escalation step runs only
/// when the gating step left the verdict unlocked. The Full Brain
/// (transformer) verdict overrides whatever the binary said.
/// </summary>
public sealed class EnsembleEscalationStepTests
{
    private const string EnsembleRoute = "/ml/ensemble";

    [Fact]
    public async Task DecoyPayload_TransformerOverridesBinary_NotBanana()
    {
        // SPIKE anchor: "yellow plastic toy shaped like a banana" -- binary
        // is permissive (~0.50 -> banana), transformer rejects (~0.44 -> not_banana).
        var fake = MakeTransformerFake(bananaScore: 0.44, label: "not_banana");
        var ctx = MakeUnlockedContext("{\"text\":\"yellow plastic toy shaped like a banana\"}");
        var step = new EnsembleEscalationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.True(ctx.Ensemble!.VerdictLocked);
        Assert.True(ctx.Ensemble.DidEscalate);
        Assert.Equal("not_banana", ctx.Ensemble.Label);
        Assert.InRange(ctx.Ensemble.Score, 0.55, 0.57); // 1 - 0.44
        Assert.False(ctx.Ensemble.Degraded);
    }

    [Fact]
    public async Task AmbiguousYellowFruit_TransformerConfirmsBanana()
    {
        var fake = MakeTransformerFake(bananaScore: 0.83, label: "banana");
        var ctx = MakeUnlockedContext("{\"text\":\"yellow fruit on the counter maybe\"}");
        var step = new EnsembleEscalationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.True(ctx.Ensemble!.DidEscalate);
        Assert.Equal("banana", ctx.Ensemble.Label);
        Assert.InRange(ctx.Ensemble.Score, 0.82, 0.84);
    }

    [Fact]
    public async Task EmptyText_TransformerDecidesBanana()
    {
        var fake = MakeTransformerFake(bananaScore: 0.65, label: "banana");
        var ctx = MakeUnlockedContext("{\"text\":\"\"}");
        var step = new EnsembleEscalationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.True(ctx.Ensemble!.DidEscalate);
        Assert.Equal("banana", ctx.Ensemble.Label);
    }

    [Fact]
    public async Task LockedVerdict_IsNotMutated()
    {
        var fake = MakeTransformerFake(bananaScore: 0.99, label: "banana");
        var ctx = new PipelineContext
        {
            Route = EnsembleRoute,
            InputJson = "{\"text\":\"x\"}",
            Ensemble = new EnsembleWorkingVerdict
            {
                Label = "not_banana",
                Score = 0.95,
                VerdictLocked = true,
                DidEscalate = false,
            },
        };
        var step = new EnsembleEscalationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.Equal("not_banana", ctx.Ensemble!.Label);
        Assert.False(ctx.Ensemble.DidEscalate);
    }

    [Fact]
    public async Task NonOkTransformerStatus_FinalizesAsUnknownDegraded()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaTransformerStatus = NativeStatusCode.InvalidArgument,
            ClassifyBananaTransformerJson = string.Empty,
        };
        var ctx = MakeUnlockedContext("{\"text\":\"x\"}");
        var step = new EnsembleEscalationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.True(ctx.Ensemble!.VerdictLocked);
        Assert.True(ctx.Ensemble.Degraded);
        Assert.True(ctx.Ensemble.DidEscalate);
        Assert.Equal("unknown", ctx.Ensemble.Label);
    }

    [Fact]
    public async Task NonEnsembleRoute_IsNoOp()
    {
        var fake = MakeTransformerFake(bananaScore: 0.99, label: "banana");
        var ctx = new PipelineContext { Route = "/ml/binary" };
        var step = new EnsembleEscalationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.Null(ctx.Ensemble);
    }

    private static FakeNativeBananaClient MakeTransformerFake(double bananaScore, string label) => new()
    {
        ClassifyBananaTransformerStatus = NativeStatusCode.Ok,
        ClassifyBananaTransformerJson =
            $"{{\"model\":\"transformer\",\"label\":\"{label}\",\"banana_score\":{bananaScore.ToString(System.Globalization.CultureInfo.InvariantCulture)}}}",
    };

    private static PipelineContext MakeUnlockedContext(string inputJson) => new()
    {
        Route = EnsembleRoute,
        InputJson = inputJson,
        Ensemble = new EnsembleWorkingVerdict { VerdictLocked = false },
    };
}
