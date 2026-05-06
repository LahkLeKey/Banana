using Microsoft.AspNetCore.Mvc;
using Banana.Api.Telemetry;

namespace Banana.Api.Controllers;

/// <summary>
/// Telemetry configuration endpoint (spec #171 — Spike 166 telemetry reduction).
/// </summary>
[ApiController]
[Route("operator")]
public sealed class TelemetryController(TelemetryEventStore store) : ControllerBase
{
    // Default sample rate: 100% (all events captured). In a future slice
    // this will be driven by environment config or a runtime store.
    private static readonly int DefaultSampleRate = int.TryParse(
        Environment.GetEnvironmentVariable("BANANA_TELEMETRY_SAMPLE_RATE"), out var r) ? r : 100;

    /// <summary>Returns the current telemetry sampling configuration.</summary>
    /// <response code="200"><c>{ sample_rate: number, unit: "percent" }</c></response>
    [HttpGet("telemetry/config")]
    [Produces("application/json")]
    public IActionResult GetConfig()
    {
        return Ok(new { sample_rate = DefaultSampleRate, unit = "percent" });
    }

    public sealed record IngestTelemetryEventRequest(
        string Source,
        string Event,
        long Timestamp,
        string Status,
        double? DurationMs,
        int? Code,
        string? Channel,
        string? Variant,
        string? Layer,
        Dictionary<string, object?>? Details);

    /// <summary>Ingests a telemetry event for persistent operator observability.</summary>
    /// <response code="202">Event accepted and persisted to postgres or fallback memory.</response>
    [HttpPost("telemetry/events")]
    [Produces("application/json")]
    public async Task<IActionResult> IngestEvent([FromBody] IngestTelemetryEventRequest request,
                                                 CancellationToken cancellationToken)
    {
        if (string.IsNullOrWhiteSpace(request.Source)
            || string.IsNullOrWhiteSpace(request.Event)
            || string.IsNullOrWhiteSpace(request.Status))
        {
            return BadRequest(new { error = new { message = "source, event, and status are required" } });
        }

        var backend = await store.AppendAsync(new TelemetryEventRecord(
            request.Source,
            request.Event,
            request.Timestamp,
            request.Status,
            request.DurationMs,
            request.Code,
            request.Channel,
            request.Variant,
            request.Layer,
            request.Details), cancellationToken);

        return Accepted(new { persisted = true, backend });
    }

    /// <summary>Returns recent telemetry events for operator dashboard hydration.</summary>
    /// <param name="limit">Maximum event count (1-1000, default 200).</param>
    /// <param name="source">Optional source filter (runtime, api, frontend, wasm-worker, native).</param>
    /// <param name="cancellationToken">Request cancellation token.</param>
    [HttpGet("telemetry/events")]
    [Produces("application/json")]
    public async Task<IActionResult> GetEvents([FromQuery] int limit = 200,
                                               [FromQuery] string? source = null,
                                               CancellationToken cancellationToken = default)
    {
        var (events, backend) = await store.ReadRecentAsync(limit, source, cancellationToken);
        return Ok(new
        {
            count = events.Count,
            backend,
            events = events.Select(e => new
            {
                source = e.Source,
                @event = e.Event,
                timestamp = e.Timestamp,
                status = e.Status,
                durationMs = e.DurationMs,
                code = e.Code,
                channel = e.Channel,
                variant = e.Variant,
                layer = e.Layer,
                details = e.Details,
            }),
        });
    }
}
