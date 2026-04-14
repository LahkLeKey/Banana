using System.Net;
using System.Text.Json;

using Banana.Api.DataAccess;
using Banana.Api.NativeInterop;

using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Mvc.Testing;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.DependencyInjection.Extensions;

using Xunit;

namespace Banana.IntegrationTests;

public sealed class BananaPipelineIntegrationTests : IClassFixture<WebApplicationFactory<Program>>
{
    private readonly WebApplicationFactory<Program> _factory;

    public BananaPipelineIntegrationTests(WebApplicationFactory<Program> factory)
    {
        _factory = factory;
    }

    [Fact]
    public async Task GetBanana_UsesPipelineAndReturnsExpectedBody()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var response = await client.GetAsync("/banana?purchases=10&multiplier=2");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        using var document = JsonDocument.Parse(body);
        var root = document.RootElement;

        Assert.Equal(10, root.GetProperty("purchases").GetInt32());
        Assert.Equal(2, root.GetProperty("multiplier").GetInt32());
        Assert.Equal(20, root.GetProperty("banana").GetInt32());
        Assert.Equal("purchases=10 multiplier=2 banana=20", root.GetProperty("message").GetString());
    }

    [Fact]
    public async Task GetBanana_WhenValidationFails_ReturnsBadRequest()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var response = await client.GetAsync("/banana?purchases=-1&multiplier=2");

        Assert.Equal(HttpStatusCode.BadRequest, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("purchases and multiplier must be non-negative.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task GetBanana_WhenNativeStepThrowsNativeInteropException_ReturnsInternalServerError()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var response = await client.GetAsync("/banana?purchases=1&multiplier=777");

        Assert.Equal(HttpStatusCode.InternalServerError, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("Native interop failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task GetBanana_WhenUnhandledExceptionOccurs_ReturnsInternalServerError()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var response = await client.GetAsync("/banana?purchases=1&multiplier=888");

        Assert.Equal(HttpStatusCode.InternalServerError, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("Unexpected server error.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task GetBanana_WhenDatabaseStepThrowsDatabaseAccessException_ReturnsServiceUnavailable()
    {
        using var client = CreateFactoryWithFailingDatabase().CreateClient();

        var response = await client.GetAsync("/banana?purchases=10&multiplier=2");

        Assert.Equal(HttpStatusCode.ServiceUnavailable, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("Database access failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task GetBanana_UsesManagedNpgsqlClient_WhenPostgresConnectionIsConfigured()
    {
        var connectionString = ResolveManagedConnectionString();
        if (string.IsNullOrWhiteSpace(connectionString))
        {
            return;
        }

        using var client = CreateFactoryWithManagedDbAndFakeNative(
            connectionString,
            "select @purchases::int as purchases, @multiplier::int as multiplier, null::text as tag").CreateClient();

        var response = await client.GetAsync("/banana?purchases=6&multiplier=3");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        using var document = JsonDocument.Parse(body);
        var root = document.RootElement;
        Assert.Equal(6, root.GetProperty("purchases").GetInt32());
        Assert.Equal(3, root.GetProperty("multiplier").GetInt32());
        Assert.Equal(18, root.GetProperty("banana").GetInt32());
    }

    [Fact]
    public async Task GetBanana_UsesManagedNpgsqlClient_WhenQueryReturnsNoRows()
    {
        var connectionString = ResolveManagedConnectionString();
        if (string.IsNullOrWhiteSpace(connectionString))
        {
            return;
        }

        using var client = CreateFactoryWithManagedDbAndFakeNative(
            connectionString,
            "select @purchases::int as purchases where false").CreateClient();

        var response = await client.GetAsync("/banana?purchases=2&multiplier=4");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);
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
    public void DataAccessClientResolution_FallsBackToNativeDal_WhenModeIsOutOfRange()
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

        Assert.IsType<NativeDalDbDataAccessClient>(client);
    }

    private WebApplicationFactory<Program> CreateFactoryWithFakeNative()
    {
        return _factory.WithWebHostBuilder(static builder =>
        {
            builder.UseEnvironment("Testing");
            builder.ConfigureServices(static services =>
            {
                services.RemoveAll<INativeBananaClient>();
                services.AddScoped<INativeBananaClient, FakeNativeBananaClient>();
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
                services.RemoveAll<INativeBananaClient>();
                services.AddScoped<IDataAccessPipelineClient, ThrowingDataAccessPipelineClient>();
                services.AddScoped<INativeBananaClient, FakeNativeBananaClient>();
            });
        });
    }

    private WebApplicationFactory<Program> CreateFactoryWithManagedDbAndFakeNative(string connectionString, string managedQuery)
    {
        return _factory.WithWebHostBuilder(builder =>
        {
            builder.UseEnvironment("Testing");
            builder.ConfigureAppConfiguration((_, configBuilder) =>
            {
                configBuilder.AddInMemoryCollection(new Dictionary<string, string?>
                {
                    ["DbAccess:Mode"] = "ManagedNpgsql",
                    ["DbAccess:ManagedQuery"] = managedQuery,
                    ["ConnectionStrings:PostgreSQL"] = connectionString
                });
            });

            builder.ConfigureServices(static services =>
            {
                services.RemoveAll<INativeBananaClient>();
                services.AddScoped<INativeBananaClient, FakeNativeBananaClient>();
            });
        });
    }

    private static string? ResolveManagedConnectionString()
    {
        var configured = Environment.GetEnvironmentVariable("ConnectionStrings__PostgreSQL");
        if (!string.IsNullOrWhiteSpace(configured))
        {
            return configured;
        }

        var pgConnection = Environment.GetEnvironmentVariable("BANANA_PG_CONNECTION");
        if (string.IsNullOrWhiteSpace(pgConnection))
        {
            return pgConnection;
        }

        if (pgConnection.Contains(';', StringComparison.Ordinal))
        {
            return pgConnection;
        }

        var segments = pgConnection.Split(' ', StringSplitOptions.RemoveEmptyEntries);
        if (segments.Length == 0 || segments.Any(static segment => !segment.Contains('=', StringComparison.Ordinal)))
        {
            return pgConnection;
        }

        static string MapKey(string key)
        {
            return key.ToLowerInvariant() switch
            {
                "user" => "Username",
                "dbname" => "Database",
                "password" => "Password",
                "host" => "Host",
                "port" => "Port",
                _ => key
            };
        }

        var mapped = segments.Select(static segment =>
        {
            var split = segment.Split('=', 2, StringSplitOptions.None);
            return $"{MapKey(split[0])}={split[1]}";
        });

        return string.Join(';', mapped);
    }

    private sealed class FakeNativeBananaClient : INativeBananaClient
    {
        public BananaResult Calculate(int purchases, int multiplier)
        {
            return multiplier switch
            {
                777 => throw new NativeInteropException("Injected native failure"),
                888 => throw new InvalidOperationException("Injected unhandled failure"),
                _ => new BananaResult(
                    purchases,
                    multiplier,
                    purchases * multiplier,
                    $"purchases={purchases} multiplier={multiplier} banana={purchases * multiplier}")
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
