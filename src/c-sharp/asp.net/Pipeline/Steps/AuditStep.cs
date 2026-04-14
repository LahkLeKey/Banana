namespace CInteropSharp.Api.Pipeline.Steps;

/// <summary>
/// Final pipeline stage that emits structured logs for diagnostics and auditability.
/// </summary>
public sealed class AuditStep : IPipelineStep<PipelineContext>
{
    private readonly ILogger<AuditStep> _logger;

    /// <summary>
    /// Initializes a new instance of the <see cref="AuditStep"/> class.
    /// </summary>
    /// <param name="logger">Logger used for structured pipeline telemetry.</param>
    public AuditStep(ILogger<AuditStep> logger)
    {
        _logger = logger;
    }

    /// <inheritdoc />
    public int Order => 400;

    /// <inheritdoc />
    public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
    {
        var output = next(input);

        output.Metadata.TryGetValue("bonus.banana", out var bonusBanana);

        _logger.LogInformation(
            "Banana pipeline executed for purchases={Purchases}, multiplier={Multiplier}, nativeBanana={NativeBanana}, bonusBanana={BonusBanana}",
            output.Purchases,
            output.Multiplier,
            output.NativeResult,
            bonusBanana ?? 0);

        return output;
    }
}
