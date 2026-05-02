using System.Security.Cryptography;
using System.Text;

namespace Banana.Api.Pipeline.Steps;

/// <summary>
/// Spec 080 — tags the PipelineContext with A/B variant based on a hash of the
/// request correlation ID (sticky per request ID). Group A uses the primary
/// model path; group B applies a +0.05 threshold shift downstream.
/// Runs after input validation (Order: 150).
/// </summary>
public sealed class AbVariantStep(ILogger<AbVariantStep> logger) : IPipelineStep<PipelineContext>
{
    public int Order => 150;

    public Task<PipelineStepResult> ExecuteAsync(PipelineContext context, CancellationToken ct)
    {
        var requestId = context.CorrelationId ?? Guid.NewGuid().ToString();
        var hashBytes = SHA256.HashData(Encoding.UTF8.GetBytes(requestId));
        var bucket = (int)(BitConverter.ToUInt32(hashBytes, 0) % 100);
        var variant = bucket < 50 ? "A" : "B";

        context.AbVariant = variant;

        logger.LogInformation(
            "AbVariant requestId={RequestId} variant={Variant}",
            requestId,
            variant);

        return Task.FromResult(PipelineStepResult.Ok());
    }
}
