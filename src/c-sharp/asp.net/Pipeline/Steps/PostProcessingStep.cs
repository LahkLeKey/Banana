namespace Banana.Api.Pipeline.Steps;

/// <summary>
/// Applies simple business enrichment after native calculation, without changing native behavior.
/// </summary>
public sealed class PostProcessingStep : IPipelineStep<PipelineContext>
{
    /// <inheritdoc />
    public int Order => 300;

    /// <inheritdoc />
    public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
    {
        var bonusBanana = input.Purchases >= 10 ? input.Multiplier * 5 : 0;

        input.Metadata["bonus.banana"] = bonusBanana;
        input.Metadata["bonus.applied"] = bonusBanana > 0;

        return next(input);
    }
}
