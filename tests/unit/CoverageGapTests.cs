using System.Reflection;
using System.Text;

using CInteropSharp.Api.Controllers;
using CInteropSharp.Api.DataAccess;
using CInteropSharp.Api.Middleware;
using CInteropSharp.Api.Models;
using CInteropSharp.Api.NativeInterop;
using CInteropSharp.Api.Pipeline;
using CInteropSharp.Api.Pipeline.Steps;
using CInteropSharp.Api.Services;

using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging.Abstractions;
using Microsoft.Extensions.Options;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class CoverageGapTests
{
    [Fact]
    public void ValidationStep_Order_IsExpected()
    {
        var step = new ValidationStep();
        Assert.Equal(100, step.Order);
    }

    [Fact]
    public void ValidationStep_Execute_ThrowsForNegativeMultiplier()
    {
        var step = new ValidationStep();

        Assert.Throws<ClientInputException>(() => step.Execute(
            new PipelineContext { Purchases = 1, Multiplier = -1 },
            static context => context));
    }

    [Fact]
    public void ValidationStep_Execute_ThrowsForNegativePurchases()
    {
        var step = new ValidationStep();

        Assert.Throws<ClientInputException>(() => step.Execute(
            new PipelineContext { Purchases = -1, Multiplier = 1 },
            static context => context));
    }

    [Fact]
    public void ValidationStep_Execute_ForValidInput_CallsNext()
    {
        var step = new ValidationStep();
        var called = false;

        var result = step.Execute(
            new PipelineContext { Purchases = 1, Multiplier = 1 },
            context =>
            {
                called = true;
                return context;
            });

        Assert.True(called);
        Assert.Equal(1, result.Purchases);
    }

    [Fact]
    public void DatabaseAccessStep_Order_IsExpected()
    {
        var step = new DatabaseAccessStep(new FakeDataAccessPipelineClient());
        Assert.Equal(150, step.Order);
    }

    [Fact]
    public void NativeCalculationStep_Order_IsExpected()
    {
        var step = new NativeCalculationStep(new FakeNativeBananaClient());
        Assert.Equal(200, step.Order);
    }

    [Fact]
    public void NativeCalculationStep_Execute_StoresNativeResultOnContext()
    {
        var step = new NativeCalculationStep(new FakeNativeBananaClient());
        var input = new PipelineContext { Purchases = 3, Multiplier = 7 };

        var result = step.Execute(input, static context => context);

        Assert.Equal(21, result.NativeResult);
        Assert.NotNull(result.NativeBananaResult);
        Assert.Equal("purchases=3 multiplier=7 banana=21", result.NativeBananaResult!.Message);
    }

    [Fact]
    public void PostProcessingStep_Order_IsExpected()
    {
        var step = new PostProcessingStep();
        Assert.Equal(300, step.Order);
    }

    [Fact]
    public void AuditStep_Order_IsExpected()
    {
        var step = new AuditStep(NullLogger<AuditStep>.Instance);
        Assert.Equal(400, step.Order);
    }

    [Fact]
    public void BananaController_Get_ReturnsExpectedPayload()
    {
        var controller = new BananaController(new FakeBananaService());

        var actionResult = controller.Get(8, 4);

        var ok = Assert.IsType<OkObjectResult>(actionResult.Result);
        var payload = Assert.IsType<BananaResponse>(ok.Value);
        Assert.Equal(8, payload.Purchases);
        Assert.Equal(4, payload.Multiplier);
        Assert.Equal(32, payload.Banana);
        Assert.Equal("ok", payload.Message);
    }

    [Fact]
    public void BananaResponse_ExposesRecordProperties()
    {
        var response = new BananaResponse(10, 2, 150, "done");

        Assert.Equal(10, response.Purchases);
        Assert.Equal(2, response.Multiplier);
        Assert.Equal(150, response.Banana);
        Assert.Equal("done", response.Message);
    }

    [Fact]
    public void DbAccessOptions_DefaultsToLegacyModeAndHasQueryTemplate()
    {
        var options = new DbAccessOptions();

        Assert.Equal(DbAccessMode.LegacyNative, options.Mode);
        Assert.Contains("@purchases", options.ManagedQuery, StringComparison.Ordinal);
    }

    [Fact]
    public async Task ErrorHandlingMiddleware_MapsClientInputExceptionToBadRequest()
    {
        var middleware = CreateMiddleware(_ => throw new ClientInputException("bad input"));
        var context = CreateHttpContext();

        await middleware.Invoke(context);

        Assert.Equal(StatusCodes.Status400BadRequest, context.Response.StatusCode);
        var body = await ReadResponseBody(context);
        Assert.Contains("bad input", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task ErrorHandlingMiddleware_MapsNativeInteropExceptionToInternalServerError()
    {
        var middleware = CreateMiddleware(_ => throw new NativeInteropException("native fail"));
        var context = CreateHttpContext();

        await middleware.Invoke(context);

        Assert.Equal(StatusCodes.Status500InternalServerError, context.Response.StatusCode);
        var body = await ReadResponseBody(context);
        Assert.Contains("Native interop failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task ErrorHandlingMiddleware_MapsDatabaseExceptionToServiceUnavailable()
    {
        var middleware = CreateMiddleware(_ => throw new DatabaseAccessException("db fail"));
        var context = CreateHttpContext();

        await middleware.Invoke(context);

        Assert.Equal(StatusCodes.Status503ServiceUnavailable, context.Response.StatusCode);
        var body = await ReadResponseBody(context);
        Assert.Contains("Database access failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task ErrorHandlingMiddleware_MapsUnhandledExceptionToInternalServerError()
    {
        var middleware = CreateMiddleware(_ => throw new InvalidOperationException("oops"));
        var context = CreateHttpContext();

        await middleware.Invoke(context);

        Assert.Equal(StatusCodes.Status500InternalServerError, context.Response.StatusCode);
        Assert.Equal("application/json", context.Response.ContentType);
        var body = await ReadResponseBody(context);
        Assert.Contains("Unexpected server error.", body, StringComparison.Ordinal);
    }

    [Fact]
    public void LegacyNativeDbDataAccessClient_EnsureSuccess_ThrowsMappedMessages()
    {
        var ensureSuccess = typeof(LegacyNativeDbDataAccessClient)
            .GetMethod("EnsureSuccess", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(ensureSuccess);

        AssertStatusMessage(ensureSuccess, NativeStatusCode.InvalidArgument, "invalid input");
        AssertStatusMessage(ensureSuccess, NativeStatusCode.DbNotConfigured, "not configured");
        AssertStatusMessage(ensureSuccess, NativeStatusCode.DbError, "query execution failed");
        AssertStatusMessage(ensureSuccess, NativeStatusCode.InternalError, "Native DB call failed");
    }

    [Fact]
    public void LegacyNativeDbDataAccessClient_ReadNullTerminatedUtf8_ThrowsWhenPointerIsZero()
    {
        var method = typeof(LegacyNativeDbDataAccessClient)
            .GetMethod("ReadNullTerminatedUtf8", BindingFlags.NonPublic | BindingFlags.Static);
        Assert.NotNull(method);

        var ex = Assert.Throws<TargetInvocationException>(() => method.Invoke(null, new object?[] { nint.Zero }));
        var inner = Assert.IsType<DatabaseAccessException>(ex.InnerException);
        Assert.Contains("null payload", inner.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void ManagedNpgsqlDataAccessClient_Execute_UsesGenericExceptionPath_ForMalformedConnectionString()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>
            {
                ["ConnectionStrings:PostgreSQL"] = "Host=;Port=not-a-number"
            })
            .Build();
        var options = Options.Create(new DbAccessOptions { ManagedQuery = "select 1" });
        var client = new ManagedNpgsqlDataAccessClient(configuration, options);

        var ex = Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(1, 1)));
        Assert.Contains("connection failed", ex.Message, StringComparison.OrdinalIgnoreCase);
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

    private static void AssertStatusMessage(MethodInfo ensureSuccess, NativeStatusCode status, string expectedMessageFragment)
    {
        var exception = Assert.Throws<TargetInvocationException>(() => ensureSuccess.Invoke(null, new object?[] { status }));
        var inner = Assert.IsType<DatabaseAccessException>(exception.InnerException);
        Assert.Contains(expectedMessageFragment, inner.Message, StringComparison.OrdinalIgnoreCase);
    }

    private sealed class FakeNativeBananaClient : INativeBananaClient
    {
        public BananaResult Calculate(int purchases, int multiplier)
        {
            var banana = purchases * multiplier;
            return new BananaResult(purchases, multiplier, banana, $"purchases={purchases} multiplier={multiplier} banana={banana}");
        }
    }

    private sealed class FakeDataAccessPipelineClient : IDataAccessPipelineClient
    {
        public RawDbAccessResult Execute(DbAccessRequest request)
        {
            return new RawDbAccessResult("legacy-native", "{}", 0);
        }
    }

    private sealed class FakeBananaService : IBananaService
    {
        public BananaResult Calculate(int purchases, int multiplier)
        {
            return new BananaResult(purchases, multiplier, purchases * multiplier, "ok");
        }
    }
}