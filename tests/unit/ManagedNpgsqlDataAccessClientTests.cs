using Banana.Api.DataAccess;

using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Options;

using Npgsql;

using Xunit;

namespace Banana.UnitTests;

public sealed class ManagedNpgsqlDataAccessClientTests
{
    [Fact]
    public void Execute_ReturnsManagedResult_WhenExecutorReturnsSingleRow()
    {
        string? capturedConnectionString = null;
        string? capturedQuery = null;
        DbAccessRequest? capturedRequest = null;

        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>
            {
                ["ConnectionStrings:PostgreSQL"] = "Host=unused;Port=5432;Username=test;Password=test;Database=test"
            })
            .Build();
        var options = Options.Create(new DbAccessOptions
        {
            BananaProfileQuery = "select @purchases::int as purchases, @multiplier::int as multiplier, null::text as tag"
        });
        var client = new ManagedNpgsqlDataAccessClient(
            configuration,
            options,
            (connectionString, query, request) =>
            {
                capturedConnectionString = connectionString;
                capturedQuery = query;
                capturedRequest = request;

                return new Dictionary<string, object?>
                {
                    ["purchases"] = request.Purchases,
                    ["multiplier"] = request.Multiplier,
                    ["tag"] = null
                };
            });

        var result = client.Execute(new DbAccessRequest(6, 3));

        Assert.Equal("managed-npgsql", result.Source);
        Assert.Equal(1, result.RowCount);
        Assert.Equal("Host=unused;Port=5432;Username=test;Password=test;Database=test", capturedConnectionString);
        Assert.Equal(options.Value.BananaProfileQuery, capturedQuery);
        Assert.NotNull(capturedRequest);
        Assert.Equal(6, capturedRequest!.Purchases);
        Assert.Equal(3, capturedRequest.Multiplier);
        Assert.Contains("\"purchases\":6", result.Payload, StringComparison.Ordinal);
        Assert.Contains("\"multiplier\":3", result.Payload, StringComparison.Ordinal);
        Assert.Contains("\"tag\":null", result.Payload, StringComparison.Ordinal);
    }

    [Fact]
    public void Execute_ThrowsDatabaseAccessException_WhenConnectionStringMissing()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>())
            .Build();
        var options = Options.Create(new DbAccessOptions
        {
            BananaProfileQuery = "select 1"
        });
        var client = new ManagedNpgsqlDataAccessClient(configuration, options);

        Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(10, 2)));
    }

    [Fact]
    public void Execute_RethrowsDatabaseAccessException_FromExecutor()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>
            {
                ["ConnectionStrings:PostgreSQL"] = "Host=unused;Port=5432;Username=test;Password=test;Database=test"
            })
            .Build();
        var options = Options.Create(new DbAccessOptions { BananaProfileQuery = "select 1" });
        var client = new ManagedNpgsqlDataAccessClient(
            configuration,
            options,
            (_, _, request) => throw new DatabaseAccessException($"Managed PostgreSQL query for contract '{request.Contract}' returned no rows."));

        var ex = Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(2, 4)));
        Assert.Contains("returned no rows", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Execute_ThrowsDatabaseAccessException_WhenNpgsqlQueryFails()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>
            {
                ["ConnectionStrings:PostgreSQL"] = "Host=invalid-host-name;Port=5432;Username=bad;Password=bad;Database=bad;Timeout=1;Command Timeout=1"
            })
            .Build();
        var options = Options.Create(new DbAccessOptions
        {
            BananaProfileQuery = "select @purchases::int as purchases, @multiplier::int as multiplier"
        });
        var client = new ManagedNpgsqlDataAccessClient(configuration, options);

        Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(10, 2)));
    }

    [Fact]
    public void Execute_UsesGenericExceptionPath_ForMalformedConnectionString()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>
            {
                ["ConnectionStrings:PostgreSQL"] = "Host=;Port=not-a-number"
            })
            .Build();
        var options = Options.Create(new DbAccessOptions { BananaProfileQuery = "select 1" });
        var client = new ManagedNpgsqlDataAccessClient(configuration, options);

        var ex = Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(1, 1)));
        Assert.Contains("connection failed", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Execute_ThrowsDatabaseAccessException_WhenExecutorThrowsNpgsqlException()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>
            {
                ["ConnectionStrings:PostgreSQL"] = "Host=unused;Port=5432;Username=test;Password=test;Database=test"
            })
            .Build();
        var options = Options.Create(new DbAccessOptions { BananaProfileQuery = "select 1" });
        var client = new ManagedNpgsqlDataAccessClient(
            configuration,
            options,
            static (_, _, _) => throw new NpgsqlException("forced query failure"));

        var ex = Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(1, 1)));
        Assert.Contains("query failed", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Execute_ThrowsDatabaseAccessException_WhenExecutorThrowsUnexpectedException()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>
            {
                ["ConnectionStrings:PostgreSQL"] = "Host=unused;Port=5432;Username=test;Password=test;Database=test"
            })
            .Build();
        var options = Options.Create(new DbAccessOptions { BananaProfileQuery = "select 1" });
        var client = new ManagedNpgsqlDataAccessClient(
            configuration,
            options,
            static (_, _, _) => throw new InvalidOperationException("forced unexpected failure"));

        var ex = Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(1, 1)));
        Assert.Contains("connection failed", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Execute_ThrowsDatabaseAccessException_ForUnsupportedContract()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>())
            .Build();
        var options = Options.Create(new DbAccessOptions());
        var client = new ManagedNpgsqlDataAccessClient(configuration, options);

        var ex = Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(10, 2, (DbAccessContract)99)));

        Assert.Contains("does not support contract", ex.Message, StringComparison.OrdinalIgnoreCase);
    }
}
