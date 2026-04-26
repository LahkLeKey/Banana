using System.Text.Json;

using Banana.Api.NativeInterop;

namespace Banana.Api.Pipeline.Steps;

/// <summary>
/// Slice 014 -- gated cascade entry. Calls the Right Brain (binary) classifier
/// and decides whether to lock the verdict (out-of-band) or escalate to the
/// Full Brain (in-band).
///
/// Cascade band: <see cref="LowerBound"/>..<see cref="UpperBound"/> inclusive
/// on the binary banana_score. Baked in for slice 014 (no configurability).
///
/// On non-OK status from the binary classifier, the step DEGRADES rather than
/// failing: it sets the working verdict's Degraded flag and asks for
/// escalation so the Full Brain can attempt a verdict (slice 014 US4).
/// </summary>
public sealed class EnsembleGatingStep(INativeBananaClient native) : IPipelineStep<PipelineContext>
{
    public const double LowerBound = 0.35;
    public const double UpperBound = 0.65;
    private const string EnsembleRoute = "/ml/ensemble";
    private const string EnsembleWithEmbeddingRoute = "/ml/ensemble/embedding";

    public int Order => 100;

    public Task<PipelineStepResult> ExecuteAsync(PipelineContext context, CancellationToken ct)
    {
        if (!string.Equals(context.Route, EnsembleRoute, StringComparison.Ordinal)
            && !string.Equals(context.Route, EnsembleWithEmbeddingRoute, StringComparison.Ordinal))
        {
            return Task.FromResult(PipelineStepResult.Ok());
        }

        context.Ensemble ??= new EnsembleWorkingVerdict();
        var working = context.Ensemble;

        var rc = native.ClassifyBananaBinary(context.InputJson ?? string.Empty, out var json);
        context.LastStatus = rc;

        if (rc != NativeStatusCode.Ok)
        {
            // Degrade: leave verdict unlocked so the escalation step can try
            // the Full Brain.
            working.Degraded = true;
            working.VerdictLocked = false;
            return Task.FromResult(PipelineStepResult.Ok());
        }

        var (label, bananaScore) = ParseLabelAndBananaScore(json);
        if (bananaScore is null)
        {
            // Native payload is malformed -- treat as degraded and escalate.
            working.Degraded = true;
            working.VerdictLocked = false;
            return Task.FromResult(PipelineStepResult.Ok());
        }

        var inBand = bananaScore.Value >= LowerBound && bananaScore.Value <= UpperBound;
        if (inBand)
        {
            working.VerdictLocked = false;
            return Task.FromResult(PipelineStepResult.Ok());
        }

        working.Label = label ?? (bananaScore.Value >= 0.5 ? "banana" : "not_banana");
        working.Score = working.Label == "banana" ? bananaScore.Value : 1.0 - bananaScore.Value;
        working.VerdictLocked = true;
        return Task.FromResult(PipelineStepResult.Ok());
    }

    internal static (string? Label, double? BananaScore) ParseLabelAndBananaScore(string json)
    {
        if (string.IsNullOrWhiteSpace(json))
        {
            return (null, null);
        }

        try
        {
            using var doc = JsonDocument.Parse(json);
            var root = doc.RootElement;
            string? label = root.TryGetProperty("label", out var labelEl) && labelEl.ValueKind == JsonValueKind.String
                ? labelEl.GetString()
                : null;

            double? bananaScore = null;
            if (root.TryGetProperty("banana_score", out var bsEl) && bsEl.ValueKind == JsonValueKind.Number)
            {
                bananaScore = bsEl.GetDouble();
            }
            else if (root.TryGetProperty("confidence", out var confEl) && confEl.ValueKind == JsonValueKind.Number)
            {
                // Fallback when only the picked-side confidence is present:
                // banana_score == confidence iff label==banana, else 1-confidence.
                var conf = confEl.GetDouble();
                bananaScore = string.Equals(label, "banana", StringComparison.Ordinal) ? conf : 1.0 - conf;
            }

            return (label, bananaScore);
        }
        catch (JsonException)
        {
            return (null, null);
        }
    }
}
