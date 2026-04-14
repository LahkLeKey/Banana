using System.Text;

using Banana.Api.DataAccess;
using Banana.Api.Middleware;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging.Abstractions;

using Xunit;

namespace Banana.UnitTests;

public sealed class ErrorHandlingMiddlewareTests
{
    [Fact]
    public async Task Invoke_Completes_WhenNextReturnsCompletedTask()
    {
        var called = false;
        RequestDelegate next = _ =>
        {
            called = true;
            return Task.CompletedTask;
        };
        var middleware = new ErrorHandlingMiddleware(next, NullLogger<ErrorHandlingMiddleware>.Instance);
        var context = new DefaultHttpContext();

        await middleware.Invoke(context);

        Assert.True(called);
    }

    [Fact]
    public async Task Invoke_Completes_WhenNextResumesAsynchronously()
    {
        var called = false;
        RequestDelegate next = async _ =>
        {
            await Task.Yield();
            called = true;
        };
        var middleware = new ErrorHandlingMiddleware(next, NullLogger<ErrorHandlingMiddleware>.Instance);
        var context = new DefaultHttpContext();

        await middleware.Invoke(context);

        Assert.True(called);
    }

    [Fact]
    public async Task Invoke_MapsClientInputExceptionToBadRequest()
    {
        var middleware = CreateMiddleware(_ => throw new ClientInputException("bad input"));
        var context = CreateHttpContext();

        await middleware.Invoke(context);

        Assert.Equal(StatusCodes.Status400BadRequest, context.Response.StatusCode);
        var body = await ReadResponseBody(context);
        Assert.Contains("bad input", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task Invoke_MapsNativeInteropExceptionToInternalServerError()
    {
        var middleware = CreateMiddleware(_ => throw new NativeInteropException("native fail"));
        var context = CreateHttpContext();

        await middleware.Invoke(context);

        Assert.Equal(StatusCodes.Status500InternalServerError, context.Response.StatusCode);
        var body = await ReadResponseBody(context);
        Assert.Contains("Native interop failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task Invoke_MapsDatabaseExceptionToServiceUnavailable()
    {
        var middleware = CreateMiddleware(_ => throw new DatabaseAccessException("db fail"));
        var context = CreateHttpContext();

        await middleware.Invoke(context);

        Assert.Equal(StatusCodes.Status503ServiceUnavailable, context.Response.StatusCode);
        var body = await ReadResponseBody(context);
        Assert.Contains("Database access failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task Invoke_MapsUnhandledExceptionToInternalServerError()
    {
        var middleware = CreateMiddleware(_ => throw new InvalidOperationException("oops"));
        var context = CreateHttpContext();

        await middleware.Invoke(context);

        Assert.Equal(StatusCodes.Status500InternalServerError, context.Response.StatusCode);
        Assert.Equal("application/json", context.Response.ContentType);
        var body = await ReadResponseBody(context);
        Assert.Contains("Unexpected server error.", body, StringComparison.Ordinal);
    }

    private static ErrorHandlingMiddleware CreateMiddleware(RequestDelegate next)
    {
        return new ErrorHandlingMiddleware(next, NullLogger<ErrorHandlingMiddleware>.Instance);
    }

    private static DefaultHttpContext CreateHttpContext()
    {
        return new DefaultHttpContext
        {
            Response =
            {
                Body = new MemoryStream()
            }
        };
    }

    private static async Task<string> ReadResponseBody(HttpContext context)
    {
        context.Response.Body.Position = 0;
        using var reader = new StreamReader(context.Response.Body, Encoding.UTF8, leaveOpen: true);
        return await reader.ReadToEndAsync();
    }
}
