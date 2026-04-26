using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Steps;
using Banana.Api.Tests.Unit.TestDoubles;

using Xunit;

namespace Banana.Api.Tests.Unit;

/// <summary>
/// Slice 014 -- US3 (calibration) coverage. The calibration step attaches
/// the regression magnitude and MUST NOT mutate the locked label/score.
/// </summary>
public sealed class EnsembleCalibrationStepTests
{
    private const string EnsembleRoute = "/ml/ensemble";

    [Fact]
    public async Task AttachesMagnitudeInZeroToOne()
    {
        var fake = MakeFake(0.7125);
        var ctx = MakeLockedContext(label: "banana", score: 0.95);
        var step = new EnsembleCalibrationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.Equal(0.7125, ctx.Ensemble!.CalibrationMagnitude, 4);
        Assert.InRange(ctx.Ensemble.CalibrationMagnitude, 0.0, 1.0);
    }

    [Fact]
    public async Task DoesNotMutateLockedLabelOrScore()
    {
        var fake = MakeFake(0.5);
        var ctx = MakeLockedContext(label: "not_banana", score: 0.92);
        var step = new EnsembleCalibrationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.Equal("not_banana", ctx.Ensemble!.Label);
        Assert.Equal(0.92, ctx.Ensemble.Score);
    }

    [Fact]
    public async Task ClampsOutOfRangeRegressionScore()
    {
        var fake = MakeFake(1.5);
        var ctx = MakeLockedContext(label: "banana", score: 0.95);
        var step = new EnsembleCalibrationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.Equal(1.0, ctx.Ensemble!.CalibrationMagnitude);
    }

    [Fact]
    public async Task NonOkRegression_DegradesAndZerosMagnitude()
    {
        var fake = new FakeNativeBananaClient
        {
            PredictRegressionStatus = NativeStatusCode.InvalidArgument,
            PredictRegressionValue = 0.0,
        };
        var ctx = MakeLockedContext(label: "banana", score: 0.95);
        var step = new EnsembleCalibrationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.True(ctx.Ensemble!.Degraded);
        Assert.Equal(0.0, ctx.Ensemble.CalibrationMagnitude);
    }

    [Fact]
    public async Task NonEnsembleRoute_IsNoOp()
    {
        var fake = MakeFake(0.5);
        var ctx = new PipelineContext { Route = "/ml/regression" };
        var step = new EnsembleCalibrationStep(fake);

        await step.ExecuteAsync(ctx, CancellationToken.None);

        Assert.Null(ctx.Ensemble);
    }

    private static FakeNativeBananaClient MakeFake(double regressionScore) => new()
    {
        PredictRegressionStatus = NativeStatusCode.Ok,
        PredictRegressionValue = regressionScore,
    };

    private static PipelineContext MakeLockedContext(string label, double score) => new()
    {
        Route = EnsembleRoute,
        InputJson = "{\"text\":\"x\"}",
        Ensemble = new EnsembleWorkingVerdict
        {
            Label = label,
            Score = score,
            VerdictLocked = true,
        },
    };
}
