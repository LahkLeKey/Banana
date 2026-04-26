using Banana.Api.NativeInterop;
using Banana.Api.Pipeline.Mapping;
using Banana.Api.Pipeline.Results;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("not-banana")]
public sealed class NotBananaController(
    INativeBananaClient native,
    INativeJsonMapper mapper,
    PipelineContext ctx) : ControllerBase
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

        var result = mapper.Deserialize<NotBananaClassificationResult>(json);
        if (result is null)
        {
            return StatusCode(500, new { error = "invalid_native_payload" });
        }

        return Ok(result);
    }
}
