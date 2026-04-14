using CInteropSharp.Api.Services;

namespace CInteropSharp.Api.Pipeline.Steps;

/// <summary>
/// Validates request values before expensive work (such as native interop) begins.
/// </summary>
public sealed class ValidationStep : IPipelineStep<PipelineContext>
{
    /// <inheritdoc />
    public int Order => 100;

    /// <inheritdoc />
    public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
    {
        if (input.Purchases < 0 || input.Multiplier < 0)
        {
            throw new ClientInputException("purchases and multiplier must be non-negative.");
        }

        return next(input);
    }
}
