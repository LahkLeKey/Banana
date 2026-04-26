using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;
using System.Text.Json;

namespace Banana.Api.Controllers;

[ApiController]
[Route("ripeness")]
public sealed class RipenessController(
    INativeBananaClient native,
    PipelineContext ctx,
    PipelineRunner<PipelineContext> runner) : ControllerBase
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    public sealed record RipenessRequest(string InputJson);

    [HttpPost("predict")]
    public async Task<IActionResult> Predict([FromBody] RipenessRequest req, CancellationToken ct)
    {
        ctx.Route = "/ripeness/predict";
        ctx.InputJson = req.InputJson;

        var pipelineResult = await runner.RunAsync(ctx, ct);
        if (!pipelineResult.IsSuccess)
        {
            return BadRequest(pipelineResult.Problem);
        }

        var rc = native.PredictBananaRipeness(req.InputJson, out var json);
        ctx.LastStatus = rc;

        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = JsonSerializer.Deserialize<RipenessResult>(
            json,
            JsonOptions);

        if (result is null)
        {
            return StatusCode(500, new { error = "invalid_native_payload" });
        }

        return rc == NativeStatusCode.Ok
            ? Ok(result)
            : StatusMapping.ToActionResult(rc);
    }
}
