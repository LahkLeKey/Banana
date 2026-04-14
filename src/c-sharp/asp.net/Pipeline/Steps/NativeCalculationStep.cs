using CInteropSharp.Api.NativeInterop;

namespace CInteropSharp.Api.Pipeline.Steps;

/// <summary>
/// Executes the existing native interop call and stores its output on the shared pipeline context.
/// </summary>
public sealed class NativeCalculationStep : IPipelineStep<PipelineContext>
{
    private readonly INativePointsClient _nativePointsClient;

    /// <summary>
    /// Initializes a new instance of the <see cref="NativeCalculationStep"/> class.
    /// </summary>
    /// <param name="nativePointsClient">Interop client that wraps P/Invoke calls.</param>
    public NativeCalculationStep(INativePointsClient nativePointsClient)
    {
        _nativePointsClient = nativePointsClient;
    }

    /// <inheritdoc />
    public int Order => 200;

    /// <inheritdoc />
    public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
    {
        var nativeResult = _nativePointsClient.Calculate(input.Purchases, input.Multiplier);

        input.NativeResult = nativeResult.Points;
        input.NativePointsResult = nativeResult;

        return next(input);
    }
}
