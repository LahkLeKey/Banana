using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Steps;
using Banana.Api.Tests.Unit.TestDoubles;

using Xunit;

namespace Banana.Api.Tests.Unit;

/// <summary>
/// Slice 014 -- US1 (cheap path) coverage. The gating step calls the binary
/// classifier and locks the verdict when banana_score is outside
/// [0.35, 0.65]; otherwise it leaves the verdict unlocked for escalation.
/// </summary>
public sealed class EnsembleGatingStepTests
{
    private const string EnsembleRoute = "/ml/ensemble";

    [Fact]
    public async Task ClearlyBananaPayload_LocksVerdict_WithoutEscalation()
    {
        var fake = MakeFake(bananaScore: 0.95, label: "banana");
        var ctx = MakeContext("{\"text\":\"ripe banana peel smoothie banana bunch banana bread\"}");
        var step = new EnsembleGatingStep(fake);

        var result = await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.True(result.IsSuccess);
        Assert.NotNull(ctx.Ensemble);
        Assert.True(ctx.Ensemble!.VerdictLocked);
        Assert.Equal("banana", ctx.Ensemble.Label);
        Assert.InRange(ctx.Ensemble.Score, 0.94, 0.96);
        Assert.False(ctx.Ensemble.Degraded);
    }

    [Fact]
    public async Task ClearlyNotBananaPayload_LocksVerdict_WithoutEscalation()
    {
        var fake = MakeFake(bananaScore: 0.05, label: "not_banana");
        var ctx = MakeContext("{\"text\":\"plastic engine oil junk waste motor oil\"}");
        var step = new EnsembleGatingStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.True(ctx.Ensemble!.VerdictLocked);
        Assert.Equal("not_banana", ctx.Ensemble.Label);
        Assert.InRange(ctx.Ensemble.Score, 0.94, 0.96);
    }

    [Fact]
    public async Task InBandPayload_LeavesVerdictUnlocked()
    {
        var fake = MakeFake(bananaScore: 0.50, label: "banana");
        var ctx = MakeContext("{\"text\":\"yellow plastic toy shaped like a banana\"}");
        var step = new EnsembleGatingStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.NotNull(ctx.Ensemble);
        Assert.False(ctx.Ensemble!.VerdictLocked);
        Assert.False(ctx.Ensemble.Degraded);
    }

    [Fact]
    public async Task InBandUpperEdge_AtZeroPointSixFive_LeavesUnlocked()
    {
        var fake = MakeFake(bananaScore: 0.65, label: "banana");
        var ctx = MakeContext("{\"text\":\"edge case\"}");
        var step = new EnsembleGatingStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.False(ctx.Ensemble!.VerdictLocked);
    }

    [Fact]
    public async Task InBandLowerEdge_AtZeroPointThreeFive_LeavesUnlocked()
    {
        var fake = MakeFake(bananaScore: 0.35, label: "not_banana");
        var ctx = MakeContext("{\"text\":\"edge case\"}");
        var step = new EnsembleGatingStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.False(ctx.Ensemble!.VerdictLocked);
    }

    [Fact]
    public async Task NonOkBinaryStatus_DegradesAndLeavesUnlocked()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.InvalidArgument,
            ClassifyBananaBinaryJson = string.Empty,
        };
        var ctx = MakeContext("{\"text\":\"x\"}");
        var step = new EnsembleGatingStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.True(ctx.Ensemble!.Degraded);
        Assert.False(ctx.Ensemble.VerdictLocked);
    }

    [Fact]
    public async Task NonEnsembleRoute_IsNoOp()
    {
        var fake = MakeFake(bananaScore: 0.95, label: "banana");
        var ctx = new PipelineContext { Route = "/ml/binary", InputJson = "{}" };
        var step = new EnsembleGatingStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.Null(ctx.Ensemble);
    }

    private static FakeNativeBananaClient MakeFake(double bananaScore, string label) => new()
    {
        ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
        ClassifyBananaBinaryJson =
            $"{{\"model\":\"binary\",\"label\":\"{label}\",\"banana_score\":{bananaScore.ToString(System.Globalization.CultureInfo.InvariantCulture)}}}",
    };

    private static PipelineContext MakeContext(string inputJson) =>
        new() { Route = EnsembleRoute, InputJson = inputJson };
}
