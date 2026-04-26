using Banana.Api.NativeInterop;

namespace Banana.Api.Pipeline.Steps;

/// <summary>
/// Slice 014 -- gated cascade escalation. Calls the Full Brain (transformer)
/// classifier when the gating step left the verdict unlocked. The transformer
/// verdict overrides whatever the binary said (FB outranks RB on escalation).
///
/// On non-OK transformer status combined with an already-degraded gating
/// outcome, the working verdict is finalized as label="unknown" and
/// Degraded=true (slice 014 US4 floor).
/// </summary>
public sealed class EnsembleEscalationStep(INativeBananaClient native) : IPipelineStep<PipelineContext>
{
    private const string EnsembleRoute = "/ml/ensemble";

    public int Order => 200;

    public Task<PipelineStepResult> ExecuteAsync(PipelineContext context, CancellationToken ct)
    {
        if (!string.Equals(context.Route, EnsembleRoute, StringComparison.Ordinal))
        {
            return Task.FromResult(PipelineStepResult.Ok());
        }

        var working = context.Ensemble;
        if (working is null || working.VerdictLocked)
        {
            return Task.FromResult(PipelineStepResult.Ok());
        }

        var rc = native.ClassifyBananaTransformer(context.InputJson ?? string.Empty, out var json);
        context.LastStatus = rc;

        if (rc != NativeStatusCode.Ok)
        {
            working.Degraded = true;
            // Cannot finalize a label without either RB or FB; defer to
            // the calibration step + controller floor for unknown labelling.
            working.Label = "unknown";
            working.Score = 0.0;
            working.DidEscalate = true;
            working.VerdictLocked = true;
            return Task.FromResult(PipelineStepResult.Ok());
        }

        var (label, bananaScore) = EnsembleGatingStep.ParseLabelAndBananaScore(json);
        if (bananaScore is null)
        {
            working.Degraded = true;
            working.Label = "unknown";
            working.Score = 0.0;
            working.DidEscalate = true;
            working.VerdictLocked = true;
            return Task.FromResult(PipelineStepResult.Ok());
        }

        working.Label = label ?? (bananaScore.Value >= 0.5 ? "banana" : "not_banana");
        working.Score = working.Label == "banana" ? bananaScore.Value : 1.0 - bananaScore.Value;
        working.DidEscalate = true;
        working.VerdictLocked = true;
        return Task.FromResult(PipelineStepResult.Ok());
    }
}
