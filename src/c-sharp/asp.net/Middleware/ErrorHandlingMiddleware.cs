using System.Net;
using System.Text.Json;

using Banana.Api.DataAccess;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

namespace Banana.Api.Middleware;

/// <summary>
/// Converts known exceptions into consistent JSON HTTP error responses.
/// </summary>
public sealed class ErrorHandlingMiddleware
{
    private readonly RequestDelegate _next;
    private readonly ILogger<ErrorHandlingMiddleware> _logger;

    /// <summary>
    /// Initializes a new instance of the <see cref="ErrorHandlingMiddleware"/> class.
    /// </summary>
    /// <param name="next">Next middleware in the ASP.NET Core request pipeline.</param>
    /// <param name="logger">Logger used for server-side error telemetry.</param>
    public ErrorHandlingMiddleware(RequestDelegate next, ILogger<ErrorHandlingMiddleware> logger)
    {
        _next = next;
        _logger = logger;
    }

    /// <summary>
    /// Executes the middleware and maps known exception types to appropriate status codes.
    /// </summary>
    /// <param name="context">Current HTTP context.</param>
    public async Task Invoke(HttpContext context)
    {
        try
        {
            await _next(context);
        }
        catch (ClientInputException ex)
        {
            await WriteProblem(context, HttpStatusCode.BadRequest, ex.Message);
        }
        catch (EntityNotFoundException ex)
        {
            await WriteProblem(context, HttpStatusCode.NotFound, ex.Message);
        }
        catch (NativeInteropException ex)
        {
            _logger.LogError(ex, "Native interop failure");
            await WriteProblem(context, HttpStatusCode.InternalServerError, "Native interop failure.");
        }
        catch (DatabaseAccessException ex)
        {
            _logger.LogError(ex, "Database access failure");
            await WriteProblem(context, HttpStatusCode.ServiceUnavailable, "Database access failure.");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Unhandled error");
            await WriteProblem(context, HttpStatusCode.InternalServerError, "Unexpected server error.");
        }
    }

    /// <summary>
    /// Writes a minimal JSON error payload to the response body.
    /// </summary>
    private static Task WriteProblem(HttpContext context, HttpStatusCode code, string message)
    {
        context.Response.StatusCode = (int)code;
        context.Response.ContentType = "application/json";

        var payload = JsonSerializer.Serialize(new { error = message });
        return context.Response.WriteAsync(payload);
    }
}
