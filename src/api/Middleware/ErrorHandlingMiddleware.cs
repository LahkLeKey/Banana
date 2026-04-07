using System.Net;
using System.Text.Json;

using CInteropSharp.Api.NativeInterop;
using CInteropSharp.Api.Services;

namespace CInteropSharp.Api.Middleware;

public sealed class ErrorHandlingMiddleware
{
    private readonly RequestDelegate _next;
    private readonly ILogger<ErrorHandlingMiddleware> _logger;

    public ErrorHandlingMiddleware(RequestDelegate next, ILogger<ErrorHandlingMiddleware> logger)
    {
        _next = next;
        _logger = logger;
    }

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
        catch (NativeInteropException ex)
        {
            _logger.LogError(ex, "Native interop failure");
            await WriteProblem(context, HttpStatusCode.InternalServerError, "Native interop failure.");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Unhandled error");
            await WriteProblem(context, HttpStatusCode.InternalServerError, "Unexpected server error.");
        }
    }

    private static Task WriteProblem(HttpContext context, HttpStatusCode code, string message)
    {
        context.Response.StatusCode = (int)code;
        context.Response.ContentType = "application/json";

        var payload = JsonSerializer.Serialize(new { error = message });
        return context.Response.WriteAsync(payload);
    }
}
