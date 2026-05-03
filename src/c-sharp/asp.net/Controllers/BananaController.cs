using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

/// <summary>Core banana calculation endpoint.</summary>
[ApiController]
[Route("[controller]")]
public sealed class BananaController(INativeBananaClient native, PipelineContext ctx) : ControllerBase
{
    /// <summary>Calculates banana total with purchase bonus breakdown.</summary>
    /// <param name="purchases">Number of banana purchases.</param>
    /// <param name="multiplier">Score multiplier (default 1).</param>
    /// <response code="200">JSON with <c>banana</c>, <c>baseAmount</c>, and <c>bonus</c>.</response>
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
