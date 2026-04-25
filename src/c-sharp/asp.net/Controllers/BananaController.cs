using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

[ApiController]
[Route("[controller]")]
public sealed class BananaController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    [HttpGet("/banana")]
    public IActionResult Get([FromQuery] int purchases = 0, [FromQuery] int multiplier = 1)
    {
        ctx.Route = "/banana";
        var rc = native.CalculateBananaWithBreakdown(purchases, multiplier, out var total, out var baseAmount, out var bonus);
        ctx.LastStatus = rc;
        if (rc != NativeStatusCode.Ok) return StatusMapping.ToActionResult(rc);
        return Ok(new { purchases, multiplier, banana = total, baseAmount, bonus });
    }
}
