using Banana.Api.NativeInterop;

namespace Banana.Api.Pipeline.Steps;

/// <summary>
/// Slice 014 -- gated cascade calibration. Calls the Left Brain (regression)
/// model and attaches its score as the calibration magnitude. MUST NOT alter
/// the locked label/score. Runs last so calibration cost is paid exactly once
/// and never influences the verdict.
///
/// On non-OK regression status, the calibration magnitude is left at 0.0 and
/// the working verdict is marked Degraded.
/// </summary>
public sealed class EnsembleCalibrationStep(INativeBananaClient native) : IPipelineStep<PipelineContext>
{
    private const string EnsembleRoute = "/ml/ensemble";

    public int Order => 300;

    public Task<PipelineStepResult> ExecuteAsync(PipelineContext context, CancellationToken ct)
    {
        if (!string.Equals(context.Route, EnsembleRoute, StringComparison.Ordinal))
        {
            return Task.FromResult(PipelineStepResult.Ok());
        }

        context.Ensemble ??= new EnsembleWorkingVerdict();
        var working = context.Ensemble;

        var rc = native.PredictRegressionScore(context.InputJson ?? string.Empty, out var score);
        context.LastStatus = rc;

        if (rc != NativeStatusCode.Ok)
        {
            working.Degraded = true;
            working.CalibrationMagnitude = 0.0;
            return Task.FromResult(PipelineStepResult.Ok());
        }

        // Clamp to [0,1] to honor the documented EnsembleVerdictResult contract
        // even if the native returns an out-of-range probability.
        if (score < 0.0) score = 0.0;
        if (score > 1.0) score = 1.0;
        working.CalibrationMagnitude = score;
        return Task.FromResult(PipelineStepResult.Ok());
    }
}
