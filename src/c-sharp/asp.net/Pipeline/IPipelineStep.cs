namespace CInteropSharp.Api.Pipeline;

/// <summary>
/// Represents a single middleware-like stage in the points processing pipeline.
/// </summary>
/// <typeparam name="T">The shared context object that flows through every pipeline step.</typeparam>
public interface IPipelineStep<T>
{
    /// <summary>
    /// Gets the execution position for this step.
    /// Lower values run earlier in the pipeline.
    /// </summary>
    int Order { get; }

    /// <summary>
    /// Executes this step and optionally calls the <paramref name="next"/> delegate to continue the chain.
    /// </summary>
    /// <param name="input">The current pipeline context.</param>
    /// <param name="next">Delegate that invokes the next step in the pipeline.</param>
    /// <returns>The updated pipeline context.</returns>
    T Execute(T input, Func<T, T> next);
}
