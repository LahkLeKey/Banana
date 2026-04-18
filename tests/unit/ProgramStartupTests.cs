using System.Net;

using Banana.Api.NativeInterop;
using Banana.Api.DataAccess;

using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Mvc.Testing;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;

using Xunit;

namespace Banana.UnitTests;

public sealed class ProgramStartupTests : IClassFixture<WebApplicationFactory<Program>>
{
    private readonly WebApplicationFactory<Program> _factory;

    public ProgramStartupTests(WebApplicationFactory<Program> factory)
    {
        EnsureNativePathConfigured();
        _factory = factory;
    }

    [Fact]
    public async Task AppStarts_AndServesSwaggerDocument()
    {
        using var client = _factory.CreateClient();

        var response = await client.GetAsync("/swagger/v1/swagger.json");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);
    }

    [Fact]
    public void Program_ResolvesNativeDalClient_WhenModeIsNativeDal()
    {
        using var factory = CreateFactoryForMode("NativeDal");
        using var scope = factory.Services.CreateScope();

        var client = scope.ServiceProvider.GetRequiredService<IDataAccessPipelineClient>();

        Assert.IsType<NativeDalDbDataAccessClient>(client);
    }

    [Fact]
    public void Program_ResolvesManagedClient_WhenModeIsManagedNpgsql()
    {
        using var factory = CreateFactoryForMode("ManagedNpgsql");
        using var scope = factory.Services.CreateScope();

        var client = scope.ServiceProvider.GetRequiredService<IDataAccessPipelineClient>();

        Assert.IsType<ManagedNpgsqlDataAccessClient>(client);
    }

    [Fact]
    public void Program_FallsBackToNativeDalClient_WhenModeIsOutOfRange()
    {
        using var factory = CreateFactoryForMode("99");
        using var scope = factory.Services.CreateScope();

        var client = scope.ServiceProvider.GetRequiredService<IDataAccessPipelineClient>();

        Assert.IsType<NativeDalDbDataAccessClient>(client);
    }

    private WebApplicationFactory<Program> CreateFactoryForMode(string mode)
    {
        return _factory.WithWebHostBuilder(builder =>
        {
            builder.UseEnvironment("Testing");
            builder.ConfigureAppConfiguration((_, configBuilder) =>
            {
                configBuilder.AddInMemoryCollection(new Dictionary<string, string?>
                {
                    ["DbAccess:Mode"] = mode
                });
            });
        });
    }

    private static void EnsureNativePathConfigured()
    {
        var root = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "../../../../../"));
        var fileName = NativeLibraryResolver.GetPlatformLibraryName();
        var candidates = new[]
        {
            Path.Combine(root, "build", "native", "bin", "Release"),
            Path.Combine(root, "build", "native", "bin")
        };

        var libraryDir = candidates.FirstOrDefault(path => File.Exists(Path.Combine(path, fileName)));
        if (libraryDir is not null)
        {
            Environment.SetEnvironmentVariable("BANANA_NATIVE_PATH", libraryDir);
            return;
        }

        var localPath = Path.Combine(AppContext.BaseDirectory, fileName);
        if (File.Exists(localPath))
        {
            return;
        }
    }
}