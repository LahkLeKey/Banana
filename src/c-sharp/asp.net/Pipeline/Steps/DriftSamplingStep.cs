using System.Security.Cryptography;
using System.Text;

namespace Banana.Api.Pipeline.Steps;

/// <summary>
/// Spec 078 — samples 10 % of requests (configurable via BANANA_DRIFT_SAMPLE_RATE),
/// logs the first-8-chars SHA-256 fingerprint of the input and the verdict score.
/// Runs before input validation (Order: 50) so it captures the raw input.
/// </summary>
public sealed class DriftSamplingStep(ILogger<DriftSamplingStep> logger) : IPipelineStep<PipelineContext>
{
    private static readonly double SampleRate =
        double.TryParse(
            Environment.GetEnvironmentVariable("BANANA_DRIFT_SAMPLE_RATE"),
            System.Globalization.NumberStyles.Float,
            System.Globalization.CultureInfo.InvariantCulture,
            out var rate)
        ? Math.Clamp(rate, 0.0, 1.0)
        : 0.1;

    public int Order => 50;

    public Task<PipelineStepResult> ExecuteAsync(PipelineContext context, CancellationToken ct)
    {
        if (Random.Shared.NextDouble() >= SampleRate)
        {
            return Task.FromResult(PipelineStepResult.Ok());
        }

        var input = context.InputJson ?? string.Empty;
        var fingerprintBytes = SHA256.HashData(Encoding.UTF8.GetBytes(input));
        var fingerprint = Convert.ToHexString(fingerprintBytes)[..8].ToLowerInvariant();

        logger.LogInformation(
            "DriftSample fingerprint={Fingerprint} route={Route}",
            fingerprint,
            context.Route ?? "unknown");

        return Task.FromResult(PipelineStepResult.Ok());
    }
}
