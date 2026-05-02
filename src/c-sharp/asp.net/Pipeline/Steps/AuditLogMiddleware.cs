using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using System.Collections.Concurrent;
using System.Security.Claims;

namespace Banana.Api.Pipeline.Steps;

/// <summary>
/// Audit record captured per write request (v1 in-memory store).
/// </summary>
public sealed record AuditRecord(
    string Method,
    string Path,
    string? Actor,
    int StatusCode,
    DateTimeOffset Timestamp);

/// <summary>
/// Thread-safe singleton store for audit records. Register as singleton in DI.
/// </summary>
public sealed class AuditStore
{
    public ConcurrentQueue<AuditRecord> Records { get; } = new();
}

/// <summary>
/// IMiddleware that captures write-operation audit records (spec #069, v1).
/// Logs via ILogger and enqueues to AuditStore for operator read-back.
/// </summary>
public sealed partial class AuditLogMiddleware(
    AuditStore store,
    ILogger<AuditLogMiddleware> logger) : IMiddleware
{
    private static readonly HashSet<string> WriteMethods =
        new(StringComparer.OrdinalIgnoreCase) { "POST", "PUT", "PATCH", "DELETE" };

    [LoggerMessage(Level = LogLevel.Information,
        Message = "AUDIT {Method} {Path} actor={Actor} status={Status}")]
    private static partial void LogAuditRecord(
        ILogger logger, string method, string path, string actor, int status);

    public async Task InvokeAsync(HttpContext context, RequestDelegate next)
    {
        await next(context);

        if (!WriteMethods.Contains(context.Request.Method)) return;

        var actor = context.User?.FindFirstValue(ClaimTypes.NameIdentifier)
                    ?? context.User?.FindFirstValue("sub");

        var record = new AuditRecord(
            Method: context.Request.Method,
            Path: context.Request.Path.Value ?? string.Empty,
            Actor: actor,
            StatusCode: context.Response.StatusCode,
            Timestamp: DateTimeOffset.UtcNow);

        store.Records.Enqueue(record);

        LogAuditRecord(logger, record.Method, record.Path,
            record.Actor ?? "anonymous", record.StatusCode);
    }
}
