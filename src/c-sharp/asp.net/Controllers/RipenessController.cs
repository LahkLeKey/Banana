using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;
using System.Text.Json;

namespace Banana.Api.Controllers;

/// <summary>Banana ripeness prediction endpoint.</summary>
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

    /// <summary>Request body for ripeness prediction.</summary>
    public sealed record RipenessRequest(string InputJson);

    /// <summary>
    /// Predicts the ripeness stage of a banana sample using the native ripeness model.
    /// Runs the full ensemble pipeline before invoking the ripeness predictor.
    /// </summary>
    /// <param name="req">Ripeness input JSON.</param>
    /// <param name="ct">Cancellation token.</param>
    /// <response code="200"><c>{ label, confidence }</c> — ripeness label and confidence score 0–1.</response>
    /// <response code="400">Pipeline or input validation failed.</response>
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
