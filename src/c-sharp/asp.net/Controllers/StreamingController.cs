using System.Text;
using System.Text.Json;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

/// <summary>
/// SSE streaming verdict endpoint (spec #054).
/// GET /api/streaming/verdict?text=&lt;text&gt;
///
/// Emits Server-Sent Events:
///   data: {"type":"progress","step":"input_validation","pct":25}
///   data: {"type":"progress","step":"ensemble_scoring","pct":75}
///   data: {"type":"complete","verdict":{...}}
/// </summary>
[ApiController]
[Route("api/streaming")]
public sealed class StreamingController : ControllerBase
{
    private static readonly JsonSerializerOptions JsonOpts =
        new() { PropertyNamingPolicy = JsonNamingPolicy.CamelCase };

    [HttpGet("verdict")]
    public async Task Verdict([FromQuery] string? text, CancellationToken ct)
    {
        if (string.IsNullOrWhiteSpace(text))
        {
            Response.StatusCode = 400;
            await Response.WriteAsync("{\"error\":\"text query parameter is required\"}", ct);
            return;
        }

        Response.Headers.ContentType = "text/event-stream";
        Response.Headers.CacheControl = "no-cache";
        Response.Headers["Connection"] = "keep-alive";
        Response.Headers["X-Accel-Buffering"] = "no";

        await WriteSseEventAsync(new
        {
            type = "progress",
            step = "input_validation",
            pct = 25
        }, ct);

        await WriteSseEventAsync(new
        {
            type = "progress",
            step = "ensemble_scoring",
            pct = 75
        }, ct);

        // v1: return a placeholder verdict; future slice wires native ML call
        await WriteSseEventAsync(new
        {
            type = "complete",
            verdict = new { text, label = "unknown", score = 0.0, status = "ok" }
        }, ct);

        await Response.Body.FlushAsync(ct);
    }

    private async Task WriteSseEventAsync(object data, CancellationToken ct)
    {
        var json = JsonSerializer.Serialize(data, JsonOpts);
        var line = $"data: {json}\n\n";
        await Response.WriteAsync(line, Encoding.UTF8, ct);
        await Response.Body.FlushAsync(ct);
    }
}
