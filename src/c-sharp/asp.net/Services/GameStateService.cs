using System.Collections.Concurrent;
using Banana.Api.Controllers;

namespace Banana.Api.Services;

/// <summary>
/// In-memory game state persistence.
/// Stores world snapshots and a rolling telemetry buffer.
/// No DB dependency — suitable for demo and testing.
/// Wire up via DI in Program.cs as a singleton.
/// </summary>
public sealed class GameStateService
{
    private readonly ConcurrentDictionary<string, GameStateDto> _saves = new();
    private readonly ConcurrentQueue<GameTelemetryDto> _telemetry = new();
    private const int TelemetryBufferMax = 3600; /* ~1 hour at 1 event/sec */

    /// <summary>Persist a snapshot and return a stable save ID.</summary>
    public string Save(GameStateDto state)
    {
        var id = $"{DateTime.UtcNow:yyyyMMddHHmmss}-{Guid.NewGuid().ToString("N")[..8]}";
        _saves[id] = state with { SaveName = state.SaveName ?? id };
        return id;
    }

    /// <summary>Retrieve a previously saved snapshot (null if not found).</summary>
    public GameStateDto? Load(string saveId) =>
        _saves.TryGetValue(saveId, out var s) ? s : null;

    /// <summary>Return all known save IDs ordered newest-first.</summary>
    public IReadOnlyList<string> ListSaveIds() =>
        [.. _saves.Keys.OrderByDescending(k => k)];

    /// <summary>Append a telemetry event, trimming the oldest when over capacity.</summary>
    public void RecordTelemetry(GameTelemetryDto ev)
    {
        _telemetry.Enqueue(ev);
        while (_telemetry.Count > TelemetryBufferMax)
            _telemetry.TryDequeue(out _);
    }

    /// <summary>Return the most recent <paramref name="limit"/> events (newest first).</summary>
    public IReadOnlyList<GameTelemetryDto> GetRecentTelemetry(int limit) =>
        [.. _telemetry.Reverse().Take(limit)];
}
