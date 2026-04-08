namespace CInteropSharp.Api.Pipeline;

/// <summary>
/// Composes registered <see cref="IPipelineStep{T}"/> instances into one deterministic execution chain.
/// </summary>
/// <typeparam name="T">The context type that flows through the pipeline.</typeparam>
public sealed class PipelineExecutor<T>
{
    private readonly IReadOnlyList<IPipelineStep<T>> _orderedSteps;

    /// <summary>
    /// Creates an executor and sorts steps by <see cref="IPipelineStep{T}.Order"/>.
    /// </summary>
    /// <param name="steps">Pipeline steps resolved from dependency injection.</param>
    public PipelineExecutor(IEnumerable<IPipelineStep<T>> steps)
    {
        _orderedSteps = steps
            .OrderBy(step => step.Order)
            .ThenBy(step => step.GetType().FullName, StringComparer.Ordinal)
            .ToArray();
    }

    /// <summary>
    /// Runs all configured steps as a middleware chain.
    /// </summary>
    /// <param name="input">Initial context for the request.</param>
    /// <returns>The context after all steps have executed.</returns>
    public T Execute(T input)
    {
        Func<T, T> chain = static current => current;

        foreach (var step in _orderedSteps.Reverse())
        {
            var next = chain;
            chain = current => step.Execute(current, next);
        }

        return chain(input);
    }
}
