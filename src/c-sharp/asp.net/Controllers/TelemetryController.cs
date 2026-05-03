using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

/// <summary>
/// Telemetry configuration endpoint (spec #171 — Spike 166 telemetry reduction).
/// </summary>
[ApiController]
[Route("operator")]
public sealed class TelemetryController : ControllerBase
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
}
