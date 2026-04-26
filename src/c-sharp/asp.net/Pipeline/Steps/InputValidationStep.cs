namespace Banana.Api.Pipeline.Steps;

public sealed class InputValidationStep : IPipelineStep<PipelineContext>
{
    public int Order => 10;

    public Task<PipelineStepResult> ExecuteAsync(PipelineContext context, CancellationToken ct)
    {
        if (string.IsNullOrWhiteSpace(context.InputJson))
        {
            return Task.FromResult(PipelineStepResult.Fail("input_json is required", 400));
        }

        return Task.FromResult(PipelineStepResult.Ok());
    }
}
