using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("ripeness")]
public sealed class RipenessController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    public sealed record RipenessRequest(string InputJson);

    [HttpPost("predict")]
    public IActionResult Predict([FromBody] RipenessRequest req)
    {
        ctx.Route = "/ripeness/predict";
        var rc = native.PredictBananaRipeness(req.InputJson, out var json);
        ctx.LastStatus = rc;
        return rc == NativeStatusCode.Ok
            ? Ok(new { json })
            : StatusMapping.ToActionResult(rc);
    }
}
