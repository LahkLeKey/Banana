using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;
using System.Text.Json;

namespace Banana.Api.Controllers;

[ApiController]
[Route("not-banana")]
public sealed class NotBananaController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    public sealed record JunkRequest(string InputJson);

    [HttpPost("junk")]
    public IActionResult Junk([FromBody] JunkRequest req)
    {
        ctx.Route = "/not-banana/junk";
        var rc = native.ClassifyNotBananaJunk(req.InputJson, out var json);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok)
        {
            return StatusMapping.ToActionResult(rc);
        }

        var result = JsonSerializer.Deserialize<JsonElement>(json);
        return Ok(result);
    }
}
