using System.Net;
using System.Net.Http.Json;
using System.Text.Json;

using Banana.Api.DataAccess;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

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

        Assert.Equal("BananaProfileProjection", response.Headers.GetValues("X-Banana-Db-Contract").Single());
        Assert.Equal("integration-fake", response.Headers.GetValues("X-Banana-Db-Source").Single());
        Assert.Equal("1", response.Headers.GetValues("X-Banana-Db-RowCount").Single());
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
        Assert.Equal("BananaProfileProjection", response.Headers.GetValues("X-Banana-Db-Contract").Single());
        Assert.Equal("managed-npgsql", response.Headers.GetValues("X-Banana-Db-Source").Single());
        Assert.Equal("1", response.Headers.GetValues("X-Banana-Db-RowCount").Single());
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

        Assert.Equal(HttpStatusCode.ServiceUnavailable, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("Database access failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task GetBanana_UsesManagedNpgsqlClient_WhenQueryReturnsMoreThanOneRow()
    {
        var connectionString = ResolveManagedConnectionString();
        if (string.IsNullOrWhiteSpace(connectionString))
        {
            return;
        }

        using var client = CreateFactoryWithManagedDbAndFakeNative(
            connectionString,
            "select @purchases::int as purchases, @multiplier::int as multiplier union all select @purchases::int, @multiplier::int").CreateClient();

        var response = await client.GetAsync("/banana?purchases=2&multiplier=4");

        Assert.Equal(HttpStatusCode.ServiceUnavailable, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        Assert.Contains("Database access failure.", body, StringComparison.Ordinal);
    }

    [Fact]
    public async Task PredictRipeness_ReturnsExpectedBody()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        await client.PostAsJsonAsync("/batches/create", new
        {
            batchId = "batch-1",
            originFarm = "farm-1",
            storageTempC = 13.2,
            ethyleneExposure = 2.5,
            estimatedShelfLifeDays = 3
        });

        var response = await client.PostAsJsonAsync("/ripeness/predict", new
        {
            batchId = "batch-1",
            temperatureHistory = new[] { 12.5, 13.0, 14.2 },
            daysSinceHarvest = 5,
            ethyleneExposure = 2.5,
            mechanicalDamage = 0.1,
            storageTempC = 13.2
        });

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        using var document = JsonDocument.Parse(body);
        var root = document.RootElement;
        Assert.Equal("batch-1", root.GetProperty("batchId").GetString());
        Assert.Equal("YELLOW", root.GetProperty("predictedStage").GetString());
        Assert.Equal(48, root.GetProperty("shelfLifeHours").GetInt32());
    }

    [Fact]
    public async Task CreateBatch_AndGetBatchStatus_ReturnExpectedBody()
    {
        using var client = CreateFactoryWithFakeNative().CreateClient();

        var createResponse = await client.PostAsJsonAsync("/batches/create", new
        {
            batchId = "batch-1",
            originFarm = "farm-1",
            storageTempC = 13.2,
            ethyleneExposure = 2.5,
            estimatedShelfLifeDays = 3
        });

        Assert.Equal(HttpStatusCode.OK, createResponse.StatusCode);

        var statusResponse = await client.GetAsync("/batches/batch-1/status");
        Assert.Equal(HttpStatusCode.OK, statusResponse.StatusCode);

        var body = await statusResponse.Content.ReadAsStringAsync();
        using var document = JsonDocument.Parse(body);
        var root = document.RootElement;
        Assert.Equal("batch-1", root.GetProperty("batchId").GetString());
        Assert.Equal("farm-1", root.GetProperty("originFarm").GetString());
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
                services.RemoveAll<IDataAccessPipelineClient>();
                services.RemoveAll<INativeBananaClient>();
                services.AddSingleton<IDataAccessPipelineClient, SuccessfulDataAccessPipelineClient>();
                services.AddSingleton<INativeBananaClient, FakeNativeBananaClient>();
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

    private WebApplicationFactory<Program> CreateFactoryWithManagedDbAndFakeNative(string connectionString, string bananaProfileQuery)
    {
        return _factory.WithWebHostBuilder(builder =>
        {
            builder.UseEnvironment("Testing");
            builder.ConfigureAppConfiguration((_, configBuilder) =>
            {
                configBuilder.AddInMemoryCollection(new Dictionary<string, string?>
                {
                    ["DbAccess:Mode"] = "ManagedNpgsql",
                    ["DbAccess:BananaProfileQuery"] = bananaProfileQuery,
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
        private readonly Dictionary<string, BananaBatchRecord> _batches = new(StringComparer.Ordinal);
        private readonly Dictionary<string, BananaHarvestBatchRecord> _harvestBatches = new(StringComparer.Ordinal);
        private readonly Dictionary<string, BananaTruckRecord> _trucks = new(StringComparer.Ordinal);

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

        public BananaBatchRecord CreateBatch(string batchId, string originFarm, double storageTempC, double ethyleneExposure, int estimatedShelfLifeDays)
        {
            var batch = new BananaBatchRecord(batchId, originFarm, "PACKED", storageTempC, ethyleneExposure, estimatedShelfLifeDays);
            _batches[batchId] = batch;
            return batch;
        }

        public BananaBatchRecord GetBatchStatus(string batchId)
        {
            if (_batches.TryGetValue(batchId, out var batch))
            {
                return batch;
            }

            throw new EntityNotFoundException("Batch was not found.");
        }

        public BananaHarvestBatchRecord CreateHarvestBatch(string harvestBatchId, string fieldId, int harvestDayOrdinal)
        {
            var harvestBatch = new BananaHarvestBatchRecord(harvestBatchId, fieldId, harvestDayOrdinal, 0, 0.0);
            _harvestBatches[harvestBatchId] = harvestBatch;
            return harvestBatch;
        }

        public BananaHarvestBatchRecord AddBunchToHarvestBatch(string harvestBatchId, string bunchId, int harvestDayOrdinal, double bunchWeightKg)
        {
            if (!_harvestBatches.TryGetValue(harvestBatchId, out var harvestBatch))
            {
                throw new EntityNotFoundException("Harvest batch was not found.");
            }

            var updated = harvestBatch with
            {
                HarvestDayOrdinal = harvestDayOrdinal,
                BunchCount = harvestBatch.BunchCount + 1,
                TotalWeightKg = harvestBatch.TotalWeightKg + bunchWeightKg
            };

            _harvestBatches[harvestBatchId] = updated;
            return updated;
        }

        public BananaHarvestBatchRecord GetHarvestBatchStatus(string harvestBatchId)
        {
            if (_harvestBatches.TryGetValue(harvestBatchId, out var harvestBatch))
            {
                return harvestBatch;
            }

            throw new EntityNotFoundException("Harvest batch was not found.");
        }

        public BananaTruckRecord RegisterTruck(string truckId, string nodeId, BananaDistributionNodeType nodeType, double latitude, double longitude, double capacityKg)
        {
            var truck = new BananaTruckRecord(truckId, nodeId, nodeType.ToString().ToUpperInvariant(), latitude, longitude, capacityKg, 0.0, 0);
            _trucks[truckId] = truck;
            return truck;
        }

        public BananaTruckRecord LoadTruckContainer(string truckId, string containerId, double containerWeightKg)
        {
            if (!_trucks.TryGetValue(truckId, out var truck))
            {
                throw new EntityNotFoundException("Truck was not found.");
            }

            var updated = truck with
            {
                CurrentLoadKg = truck.CurrentLoadKg + containerWeightKg,
                ContainerCount = truck.ContainerCount + 1
            };

            _trucks[truckId] = updated;
            return updated;
        }

        public BananaTruckRecord UnloadTruckContainer(string truckId, string containerId, double containerWeightKg)
        {
            if (!_trucks.TryGetValue(truckId, out var truck))
            {
                throw new EntityNotFoundException("Truck was not found.");
            }

            var updated = truck with
            {
                CurrentLoadKg = Math.Max(0.0, truck.CurrentLoadKg - containerWeightKg),
                ContainerCount = Math.Max(0, truck.ContainerCount - 1)
            };

            _trucks[truckId] = updated;
            return updated;
        }

        public BananaTruckRecord RelocateTruck(string truckId, string nodeId, BananaDistributionNodeType nodeType, double latitude, double longitude)
        {
            if (!_trucks.TryGetValue(truckId, out var truck))
            {
                throw new EntityNotFoundException("Truck was not found.");
            }

            var updated = truck with
            {
                CurrentNodeId = nodeId,
                NodeType = nodeType.ToString().ToUpperInvariant(),
                Latitude = latitude,
                Longitude = longitude
            };

            _trucks[truckId] = updated;
            return updated;
        }

        public BananaTruckRecord GetTruckStatus(string truckId)
        {
            if (_trucks.TryGetValue(truckId, out var truck))
            {
                return truck;
            }

            throw new EntityNotFoundException("Truck was not found.");
        }

        public BananaRipenessPrediction PredictRipeness(
            IReadOnlyList<double> temperatureHistoryC,
            int daysSinceHarvest,
            double ethyleneExposure,
            double mechanicalDamage,
            double storageTempC)
        {
            return new BananaRipenessPrediction("YELLOW", 48, 180.0, 0.3, 0.15);
        }

        public BananaRipenessPrediction PredictBatchRipeness(
            string batchId,
            IReadOnlyList<double> temperatureHistoryC,
            int daysSinceHarvest,
            double mechanicalDamage)
        {
            if (!_batches.ContainsKey(batchId))
            {
                throw new EntityNotFoundException("Batch was not found.");
            }

            return new BananaRipenessPrediction("YELLOW", 48, 180.0, 0.3, 0.15);
        }
    }

    private sealed class ThrowingDataAccessPipelineClient : IDataAccessPipelineClient
    {
        public RawDbAccessResult Execute(DbAccessRequest request)
        {
            throw new DatabaseAccessException("Injected database-stage failure");
        }
    }

    private sealed class SuccessfulDataAccessPipelineClient : IDataAccessPipelineClient
    {
        public RawDbAccessResult Execute(DbAccessRequest request)
        {
            var payload = $"{{\"purchases\":{request.Purchases},\"multiplier\":{request.Multiplier}}}";
            return new RawDbAccessResult("integration-fake", payload, 1);
        }
    }
}
