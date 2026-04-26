namespace Banana.Api.Pipeline;

public sealed class PipelineRunner<T>(IEnumerable<IPipelineStep<T>> steps)
{
    private readonly IReadOnlyList<IPipelineStep<T>> _steps =
        [.. steps.OrderBy(step => step.Order)];

    public async Task<PipelineStepResult> RunAsync(T context, CancellationToken ct = default)
    {
        foreach (var step in _steps)
        {
            var result = await step.ExecuteAsync(context, ct);
            if (!result.IsSuccess)
            {
                return result;
            }
        }

        return PipelineStepResult.Ok();
    }
}
