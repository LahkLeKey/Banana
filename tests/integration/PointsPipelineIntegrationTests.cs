using System.Net;
using System.Text.Json;

using CInteropSharp.Api.DataAccess;
using CInteropSharp.Api.NativeInterop;

using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Mvc.Testing;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.DependencyInjection.Extensions;

using Xunit;

namespace CInteropSharp.IntegrationTests;

public sealed class PointsPipelineIntegrationTests : IClassFixture<WebApplicationFactory<Program>>
{
    private readonly WebApplicationFactory<Program> _factory;

    public PointsPipelineIntegrationTests(WebApplicationFactory<Program> factory)
    {
        _factory = factory;
    }

    [Fact]
    public async Task GetPoints_UsesPipelineAndReturnsExpectedBody()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var response = await client.GetAsync("/points?purchases=10&multiplier=2");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        using var document = JsonDocument.Parse(body);
        var root = document.RootElement;

        Assert.Equal(10, root.GetProperty("purchases").GetInt32());
        Assert.Equal(2, root.GetProperty("multiplier").GetInt32());
        Assert.Equal(20, root.GetProperty("points").GetInt32());
        Assert.Equal("purchases=10 multiplier=2 points=20", root.GetProperty("message").GetString());
    }

    [Fact]
    public async Task GetPoints_WhenValidationFails_ReturnsBadRequest()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var response = await client.GetAsync("/points?purchases=-1&multiplier=2");

        Assert.Equal(HttpStatusCode.BadRequest, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("purchases and multiplier must be non-negative.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task GetPoints_WhenNativeStepThrowsNativeInteropException_ReturnsInternalServerError()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var response = await client.GetAsync("/points?purchases=1&multiplier=777");

        Assert.Equal(HttpStatusCode.InternalServerError, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("Native interop failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task GetPoints_WhenUnhandledExceptionOccurs_ReturnsInternalServerError()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var response = await client.GetAsync("/points?purchases=1&multiplier=888");

        Assert.Equal(HttpStatusCode.InternalServerError, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("Unexpected server error.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task GetPoints_WhenDatabaseStepThrowsDatabaseAccessException_ReturnsServiceUnavailable()
    {
        using var client = CreateFactoryWithFailingDatabase().CreateClient();

        var response = await client.GetAsync("/points?purchases=10&multiplier=2");

        Assert.Equal(HttpStatusCode.ServiceUnavailable, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("Database access failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public void DataAccessClientResolution_UsesManagedClient_WhenConfigured()
    {
        using var factory = _factory.WithWebHostBuilder(builder =>
        {
            builder.UseEnvironment("Testing");
            builder.ConfigureAppConfiguration((_, configBuilder) =>
            {
                configBuilder.AddInMemoryCollection(new Dictionary<string, string?>
                {
                    ["DbAccess:Mode"] = "ManagedNpgsql"
                });
            });
        });

        using var scope = factory.Services.CreateScope();
        var client = scope.ServiceProvider.GetRequiredService<IDataAccessPipelineClient>();

        Assert.IsType<ManagedNpgsqlDataAccessClient>(client);
    }

    [Fact]
    public void DataAccessClientResolution_FallsBackToLegacy_WhenModeIsOutOfRange()
    {
        using var factory = _factory.WithWebHostBuilder(builder =>
        {
            builder.UseEnvironment("Testing");
            builder.ConfigureAppConfiguration((_, configBuilder) =>
            {
                configBuilder.AddInMemoryCollection(new Dictionary<string, string?>
                {
                    ["DbAccess:Mode"] = "99"
                });
            });
        });

        using var scope = factory.Services.CreateScope();
        var client = scope.ServiceProvider.GetRequiredService<IDataAccessPipelineClient>();

        Assert.IsType<LegacyNativeDbDataAccessClient>(client);
    }

    private WebApplicationFactory<Program> CreateFactoryWithFakeNative()
    {
        return _factory.WithWebHostBuilder(static builder =>
        {
            builder.UseEnvironment("Testing");
            builder.ConfigureServices(static services =>
            {
                services.RemoveAll<INativePointsClient>();
                services.AddScoped<INativePointsClient, FakeNativePointsClient>();
            });
        });
    }

    private WebApplicationFactory<Program> CreateFactoryWithFailingDatabase()
    {
        return _factory.WithWebHostBuilder(static builder =>
        {
            builder.UseEnvironment("Testing");
            builder.ConfigureServices(static services =>
            {
                services.RemoveAll<IDataAccessPipelineClient>();
                services.RemoveAll<INativePointsClient>();
                services.AddScoped<IDataAccessPipelineClient, ThrowingDataAccessPipelineClient>();
                services.AddScoped<INativePointsClient, FakeNativePointsClient>();
            });
        });
    }

    private sealed class FakeNativePointsClient : INativePointsClient
    {
        public PointsResult Calculate(int purchases, int multiplier)
        {
            return multiplier switch
            {
                777 => throw new NativeInteropException("Injected native failure"),
                888 => throw new InvalidOperationException("Injected unhandled failure"),
                _ => new PointsResult(
                    purchases,
                    multiplier,
                    purchases * multiplier,
                    $"purchases={purchases} multiplier={multiplier} points={purchases * multiplier}")
            };
        }
    }

    private sealed class ThrowingDataAccessPipelineClient : IDataAccessPipelineClient
    {
        public RawDbAccessResult Execute(DbAccessRequest request)
        {
            throw new DatabaseAccessException("Injected database-stage failure");
        }
    }
}
