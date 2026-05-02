using Banana.Api.Pipeline.Steps;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

/// <summary>
/// Operator-gated audit event read endpoint (spec #069).
/// GET /operator/audit?limit=50&cursor=
/// </summary>
[ApiController]
[Route("operator")]
public sealed class AuditController(AuditStore store) : ControllerBase
{
    public sealed record AuditQuery(int Limit = 50, int Cursor = 0);

    [HttpGet("audit")]
    [Authorize(Policy = "OperatorUp")]
    public IActionResult GetAudit([FromQuery] int limit = 50, [FromQuery] int cursor = 0)
    {
        limit = Math.Clamp(limit, 1, 200);

        var all = store.Records.ToArray();
        var page = all.Skip(cursor).Take(limit).ToArray();
        var nextCursor = cursor + page.Length < all.Length ? cursor + page.Length : (int?)null;

        return Ok(new { events = page, next_cursor = nextCursor });
    }
}
