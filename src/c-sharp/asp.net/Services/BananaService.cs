using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;

namespace Banana.Api.Services;

/// <summary>
/// Orchestrates the banana workflow by delegating execution to the configured pipeline.
/// </summary>
public sealed class BananaService : IBananaService
{
    private readonly PipelineExecutor<PipelineContext> _pipelineExecutor;

    /// <summary>
    /// Initializes a new instance of the <see cref="BananaService"/> class.
    /// </summary>
    /// <param name="pipelineExecutor">Executor that runs all configured banana pipeline steps.</param>
    public BananaService(PipelineExecutor<PipelineContext> pipelineExecutor)
    {
        _pipelineExecutor = pipelineExecutor;
    }

    /// <inheritdoc />
    public BananaResult Calculate(int purchases, int multiplier)
    {
        var context = new PipelineContext
        {
            Purchases = purchases,
            Multiplier = multiplier
        };

        var pipelineResult = _pipelineExecutor.Execute(context);

        if (pipelineResult.NativeBananaResult is null)
        {
            throw new NativeInteropException("Pipeline completed without a native calculation result.");
        }

        return pipelineResult.NativeBananaResult;
    }
}

/// <summary>
/// Represents a client-side input validation failure.
/// Middleware maps this to HTTP 400 responses.
/// </summary>
public sealed class ClientInputException : Exception
{
    /// <summary>
    /// Initializes a new instance of the <see cref="ClientInputException"/> class.
    /// </summary>
    /// <param name="message">Validation error description safe to return to clients.</param>
    public ClientInputException(string message) : base(message)
    {
    }
}
