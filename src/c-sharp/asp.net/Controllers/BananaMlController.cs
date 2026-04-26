using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;
using System.Text.Json;

namespace Banana.Api.Controllers;

[ApiController]
[Route("ml")]
public sealed class BananaMlController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    public sealed record MlRequest(string InputJson);

    [HttpPost("regression")]
    public IActionResult Regression([FromBody] MlRequest req)
    {
        ctx.Route = "/ml/regression";
        var rc = native.PredictRegressionScore(req.InputJson, out var score);
        ctx.LastStatus = rc;
        return rc == NativeStatusCode.Ok
            ? Ok(new { score })
            : StatusMapping.ToActionResult(rc);
    }

    [HttpPost("binary")]
    public IActionResult Binary([FromBody] MlRequest req)
    {
        ctx.Route = "/ml/binary";
        var rc = native.ClassifyBananaBinary(req.InputJson, out var json);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = JsonSerializer.Deserialize<JsonElement>(json);
        return Ok(result);
    }

    [HttpPost("transformer")]
    public IActionResult Transformer([FromBody] MlRequest req)
    {
        ctx.Route = "/ml/transformer";
        var rc = native.ClassifyBananaTransformer(req.InputJson, out var json);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = JsonSerializer.Deserialize<JsonElement>(json);
        return Ok(result);
    }
}