using System.Text.Json;
using Npgsql;

namespace Banana.Api.Telemetry;

public sealed record TelemetryEventRecord(
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

public sealed class TelemetryEventStore
{
    private const int MaxInMemoryEvents = 5000;

    private readonly string? _connectionString;
    private readonly List<TelemetryEventRecord> _memory = [];
    private readonly object _memoryGate = new();

    public TelemetryEventStore(IConfiguration configuration)
    {
        _connectionString = configuration["BANANA_PG_CONNECTION"]
            ?? Environment.GetEnvironmentVariable("BANANA_PG_CONNECTION")
            ?? configuration.GetConnectionString("BananaPg");
    }

    public bool DatabaseEnabled => !string.IsNullOrWhiteSpace(_connectionString);

    public async Task<string> AppendAsync(TelemetryEventRecord record, CancellationToken cancellationToken)
    {
        if (!DatabaseEnabled)
        {
            AppendToMemory(record);
            return "memory";
        }

        try
        {
            await EnsureSchemaAsync(cancellationToken);

            await using var conn = new NpgsqlConnection(_connectionString);
            await conn.OpenAsync(cancellationToken);

            await using var cmd = conn.CreateCommand();
            cmd.CommandText = @"
insert into operator_telemetry_events
    (source, event_name, status, occurred_at, duration_ms, code, channel, variant, layer, details)
values
    (@source, @event_name, @status, @occurred_at, @duration_ms, @code, @channel, @variant, @layer, @details::jsonb);
";

            cmd.Parameters.AddWithValue("source", record.Source);
            cmd.Parameters.AddWithValue("event_name", record.Event);
            cmd.Parameters.AddWithValue("status", record.Status);
            cmd.Parameters.AddWithValue("occurred_at", DateTimeOffset.FromUnixTimeMilliseconds(
                record.Timestamp > 0 ? record.Timestamp : DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()));
            cmd.Parameters.AddWithValue("duration_ms", (object?)record.DurationMs ?? DBNull.Value);
            cmd.Parameters.AddWithValue("code", (object?)record.Code ?? DBNull.Value);
            cmd.Parameters.AddWithValue("channel", (object?)record.Channel ?? DBNull.Value);
            cmd.Parameters.AddWithValue("variant", (object?)record.Variant ?? DBNull.Value);
            cmd.Parameters.AddWithValue("layer", (object?)record.Layer ?? DBNull.Value);
            cmd.Parameters.AddWithValue("details", JsonSerializer.Serialize(record.Details ?? new Dictionary<string, object?>()));

            await cmd.ExecuteNonQueryAsync(cancellationToken);
            return "postgres";
        }
        catch
        {
            AppendToMemory(record);
            return "memory";
        }
    }

    public async Task<(IReadOnlyList<TelemetryEventRecord> Events, string Backend)>
        ReadRecentAsync(int limit, string? source, CancellationToken cancellationToken)
    {
        limit = Math.Clamp(limit, 1, 1000);

        if (!DatabaseEnabled)
        {
            return (ReadFromMemory(limit, source), "memory");
        }

        try
        {
            await EnsureSchemaAsync(cancellationToken);

            await using var conn = new NpgsqlConnection(_connectionString);
            await conn.OpenAsync(cancellationToken);

            await using var cmd = conn.CreateCommand();
            cmd.CommandText = @"
select source, event_name, status, occurred_at, duration_ms, code, channel, variant, layer, details::text
from operator_telemetry_events
where (@source is null or source = @source)
order by occurred_at desc
limit @limit;
";

            cmd.Parameters.AddWithValue("source", (object?)source ?? DBNull.Value);
            cmd.Parameters.AddWithValue("limit", limit);

            var rows = new List<TelemetryEventRecord>();
            await using var reader = await cmd.ExecuteReaderAsync(cancellationToken);
            while (await reader.ReadAsync(cancellationToken))
            {
                Dictionary<string, object?>? details = null;
                if (!reader.IsDBNull(9))
                {
                    var detailsRaw = reader.GetString(9);
                    details = JsonSerializer.Deserialize<Dictionary<string, object?>>(detailsRaw);
                }

                var occurredAt = reader.GetFieldValue<DateTimeOffset>(3);
                rows.Add(new TelemetryEventRecord(
                    reader.GetString(0),
                    reader.GetString(1),
                    occurredAt.ToUnixTimeMilliseconds(),
                    reader.GetString(2),
                    reader.IsDBNull(4) ? null : reader.GetDouble(4),
                    reader.IsDBNull(5) ? null : reader.GetInt32(5),
                    reader.IsDBNull(6) ? null : reader.GetString(6),
                    reader.IsDBNull(7) ? null : reader.GetString(7),
                    reader.IsDBNull(8) ? null : reader.GetString(8),
                    details));
            }

            return (rows, "postgres");
        }
        catch
        {
            return (ReadFromMemory(limit, source), "memory");
        }
    }

    private void AppendToMemory(TelemetryEventRecord record)
    {
        lock (_memoryGate)
        {
            _memory.Add(record);
            if (_memory.Count > MaxInMemoryEvents)
            {
                _memory.RemoveRange(0, _memory.Count - MaxInMemoryEvents);
            }
        }
    }

    private TelemetryEventRecord[] ReadFromMemory(int limit, string? source)
    {
        lock (_memoryGate)
        {
            IEnumerable<TelemetryEventRecord> query = _memory;
            if (!string.IsNullOrWhiteSpace(source))
            {
                query = query.Where(r => string.Equals(r.Source, source, StringComparison.OrdinalIgnoreCase));
            }

            return query
                .OrderByDescending(r => r.Timestamp)
                .Take(limit)
                .ToArray();
        }
    }

    private async Task EnsureSchemaAsync(CancellationToken cancellationToken)
    {
        await using var conn = new NpgsqlConnection(_connectionString);
        await conn.OpenAsync(cancellationToken);

        await using var cmd = conn.CreateCommand();
        cmd.CommandText = @"
create table if not exists operator_telemetry_events (
    id bigserial primary key,
    source text not null,
    event_name text not null,
    status text not null,
    occurred_at timestamptz not null,
    duration_ms double precision null,
    code integer null,
    channel text null,
    variant text null,
    layer text null,
    details jsonb null
);

create index if not exists idx_operator_telemetry_events_occurred_at
    on operator_telemetry_events (occurred_at desc);

create index if not exists idx_operator_telemetry_events_source
    on operator_telemetry_events (source);
";

        await cmd.ExecuteNonQueryAsync(cancellationToken);
    }
}