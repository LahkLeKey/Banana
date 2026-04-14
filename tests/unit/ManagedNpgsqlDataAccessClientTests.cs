using CInteropSharp.Api.DataAccess;

using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Options;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class ManagedNpgsqlDataAccessClientTests
{
    [Fact]
    public void Execute_ThrowsDatabaseAccessException_WhenConnectionStringMissing()
    {
        var configuration = new ConfigurationBuilder()
            .AddInMemoryCollection(new Dictionary<string, string?>())
            .Build();
        var options = Options.Create(new DbAccessOptions
        {
            ManagedQuery = "select 1"
        });
        var client = new ManagedNpgsqlDataAccessClient(configuration, options);

        Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(10, 2)));
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
            ManagedQuery = "select @purchases::int as purchases, @multiplier::int as multiplier"
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
        var options = Options.Create(new DbAccessOptions { ManagedQuery = "select 1" });
        var client = new ManagedNpgsqlDataAccessClient(configuration, options);

        var ex = Assert.Throws<DatabaseAccessException>(() => client.Execute(new DbAccessRequest(1, 1)));
        Assert.Contains("connection failed", ex.Message, StringComparison.OrdinalIgnoreCase);
    }
}
