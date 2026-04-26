using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Pipeline;

public sealed record PipelineStepResult(bool IsSuccess, ProblemDetails? Problem = null)
{
    public static PipelineStepResult Ok() => new(true);

    public static PipelineStepResult Fail(string title, int status = 400) =>
        new(false, new ProblemDetails { Title = title, Status = status });
}
