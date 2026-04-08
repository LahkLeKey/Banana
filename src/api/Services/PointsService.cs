using CInteropSharp.Api.NativeInterop;
using CInteropSharp.Api.Pipeline;

namespace CInteropSharp.Api.Services;

/// <summary>
/// Orchestrates the points workflow by delegating execution to the configured pipeline.
/// </summary>
public sealed class PointsService : IPointsService
{
    private readonly PipelineExecutor<PipelineContext> _pipelineExecutor;

    /// <summary>
    /// Initializes a new instance of the <see cref="PointsService"/> class.
    /// </summary>
    /// <param name="pipelineExecutor">Executor that runs all configured points pipeline steps.</param>
    public PointsService(PipelineExecutor<PipelineContext> pipelineExecutor)
    {
        _pipelineExecutor = pipelineExecutor;
    }

    /// <inheritdoc />
    public PointsResult Calculate(int purchases, int multiplier)
    {
        var context = new PipelineContext
        {
            Purchases = purchases,
            Multiplier = multiplier
        };

        var pipelineResult = _pipelineExecutor.Execute(context);

        if (pipelineResult.NativePointsResult is null)
        {
            throw new NativeInteropException("Pipeline completed without a native calculation result.");
        }

        return pipelineResult.NativePointsResult;
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
