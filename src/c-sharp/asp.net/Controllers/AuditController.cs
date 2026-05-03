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

    /// <summary>Returns a paginated page of audit events.</summary>
    /// <param name="limit">Maximum number of events to return (1–200, default 50).</param>
    /// <param name="cursor">Zero-based offset to start reading from.</param>
    /// <response code="200">Paged audit event list with optional <c>next_cursor</c>.</response>
    /// <response code="401">Missing or invalid JWT.</response>
    /// <response code="403">Caller does not have OperatorUp role.</response>
    [HttpGet("audit")]
    [Authorize(Policy = "OperatorUp")]
    [Produces("application/json")]
    public IActionResult GetAudit([FromQuery] int limit = 50, [FromQuery] int cursor = 0)
    {
        limit = Math.Clamp(limit, 1, 200);

        var all = store.Records.ToArray();
        var page = all.Skip(cursor).Take(limit).ToArray();
        var nextCursor = cursor + page.Length < all.Length ? cursor + page.Length : (int?)null;

        return Ok(new { events = page, next_cursor = nextCursor });
    }
}
