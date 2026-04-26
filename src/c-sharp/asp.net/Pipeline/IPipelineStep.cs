namespace Banana.Api.Pipeline;

public interface IPipelineStep<T>
{
    int Order { get; }

    Task<PipelineStepResult> ExecuteAsync(T context, CancellationToken ct);
}
