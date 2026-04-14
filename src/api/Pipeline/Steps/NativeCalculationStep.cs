using CInteropSharp.Api.NativeInterop;

namespace CInteropSharp.Api.Pipeline.Steps;

/// <summary>
/// Executes the existing native interop call and stores its output on the shared pipeline context.
/// </summary>
public sealed class NativeCalculationStep : IPipelineStep<PipelineContext>
{
    private readonly INativeBananaClient _nativeBananaClient;

    /// <summary>
    /// Initializes a new instance of the <see cref="NativeCalculationStep"/> class.
    /// </summary>
    /// <param name="nativeBananaClient">Interop client that wraps P/Invoke calls.</param>
    public NativeCalculationStep(INativeBananaClient nativeBananaClient)
    {
        _nativeBananaClient = nativeBananaClient;
    }

    /// <inheritdoc />
    public int Order => 200;

    /// <inheritdoc />
    public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
    {
        var nativeResult = _nativeBananaClient.Calculate(input.Purchases, input.Multiplier);

        input.NativeResult = nativeResult.Banana;
        input.NativeBananaResult = nativeResult;

        return next(input);
    }
}
