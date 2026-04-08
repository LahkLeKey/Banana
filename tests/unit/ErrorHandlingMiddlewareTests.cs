using CInteropSharp.Api.Middleware;

using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging.Abstractions;

using Xunit;

namespace CInteropSharp.UnitTests;

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
}
