namespace CInteropSharp.Api.Pipeline.Steps;

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
        var bonusPoints = input.Purchases >= 10 ? input.Multiplier * 5 : 0;

        input.Metadata["bonus.points"] = bonusPoints;
        input.Metadata["bonus.applied"] = bonusPoints > 0;

        return next(input);
    }
}
