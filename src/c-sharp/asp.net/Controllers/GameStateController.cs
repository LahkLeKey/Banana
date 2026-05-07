using Banana.Api.Services;
using Microsoft.AspNetCore.Mvc;

namespace Banana.Api.Controllers;

/// <summary>
/// Game engine state persistence and telemetry endpoints (Phase 1e).
/// Provides save/load for world snapshots and frame telemetry ingestion.
/// </summary>
[ApiController]
[Route("api/game")]
public sealed class GameStateController(GameStateService gameState) : ControllerBase
{
    /// <summary>Persist a world state snapshot.</summary>
    /// <response code="200">JSON with <c>save_id</c>.</response>
    [HttpPost("save")]
    [Produces("application/json")]
    public IActionResult SaveState([FromBody] GameStateDto state)
    {
        if (state is null)
            return BadRequest(new { error = "state is required" });

        var saveId = gameState.Save(state);
        return Ok(new { save_id = saveId });
    }

    /// <summary>Load a previously saved world state snapshot.</summary>
    /// <param name="saveId">Save identifier returned by POST /api/game/save.</param>
    /// <response code="200">Saved <see cref="GameStateDto"/>.</response>
    /// <response code="404">Not found.</response>
    [HttpGet("load/{saveId}")]
    [Produces("application/json")]
    public IActionResult LoadState(string saveId)
    {
        var state = gameState.Load(saveId);
        if (state is null)
            return NotFound(new { error = $"save '{saveId}' not found" });
        return Ok(state);
    }

    /// <summary>List all available save identifiers.</summary>
    [HttpGet("saves")]
    [Produces("application/json")]
    public IActionResult ListSaves()
    {
        return Ok(gameState.ListSaveIds());
    }

    /// <summary>Ingest a frame telemetry event from the WASM game loop.</summary>
    /// <response code="204">Accepted.</response>
    [HttpPost("telemetry")]
    public IActionResult EmitTelemetry([FromBody] GameTelemetryDto telemetry)
    {
        if (telemetry is null)
            return BadRequest(new { error = "telemetry is required" });

        gameState.RecordTelemetry(telemetry);
        return NoContent();
    }

    /// <summary>Return the last N recorded telemetry events (most recent first).</summary>
    [HttpGet("telemetry")]
    [Produces("application/json")]
    public IActionResult GetTelemetry([FromQuery] int limit = 60)
    {
        return Ok(gameState.GetRecentTelemetry(Math.Clamp(limit, 1, 1000)));
    }
}

/* ── DTOs ──────────────────────────────────────────────────────────────────── */

public sealed record EntitySnapshotDto(
    int Id,
    string Type,
    float[] Position,
    float[] Rotation,
    int State
);

public sealed record GameStateDto(
    string? SaveName,
    EntitySnapshotDto[] Entities,
    long Timestamp
);

public sealed record GameTelemetryDto(
    int FrameCount,
    int EntityCount,
    float AvgFrameMs,
    float MinFrameMs,
    float MaxFrameMs,
    long Timestamp
);
