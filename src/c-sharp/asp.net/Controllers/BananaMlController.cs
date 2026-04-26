using Banana.Api.NativeInterop;
using Banana.Api.Pipeline.Mapping;
using Banana.Api.Pipeline.Results;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("ml")]
public sealed class BananaMlController(
    INativeBananaClient native,
    INativeJsonMapper mapper,
    PipelineContext ctx) : ControllerBase
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

        var result = mapper.Deserialize<BinaryClassificationResult>(json);
        if (result is null)
        {
            return StatusCode(500, new { error = "invalid_native_payload" });
        }

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

        var result = mapper.Deserialize<TransformerClassificationResult>(json);
        if (result is null)
        {
            return StatusCode(500, new { error = "invalid_native_payload" });
        }

        return Ok(result);
    }
}