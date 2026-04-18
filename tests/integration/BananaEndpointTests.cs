using System.Net;
using System.Runtime.InteropServices;
using System.Text.Json;

using Banana.Api.NativeInterop;

using Microsoft.AspNetCore.Mvc.Testing;

using Xunit;

namespace Banana.IntegrationTests;

public sealed class BananaEndpointTests : IClassFixture<WebApplicationFactory<Program>>
{
    private readonly WebApplicationFactory<Program> _factory;

    public BananaEndpointTests(WebApplicationFactory<Program> factory)
    {
        _factory = factory;
    }

    [Fact]
    public async Task GetBanana_ReturnsOk_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        using var client = _factory.CreateClient();
        var response = await client.GetAsync("/banana?purchases=10&multiplier=2");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);
    }

    [Fact]
    public async Task GetBanana_ReturnsExpectedResponseBody_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        using var client = _factory.CreateClient();
        var response = await client.GetAsync("/banana?purchases=10&multiplier=2");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);

        var body = await response.Content.ReadAsStringAsync();
        using var document = JsonDocument.Parse(body);
        var root = document.RootElement;

        Assert.Equal(10, root.GetProperty("purchases").GetInt32());
        Assert.Equal(2, root.GetProperty("multiplier").GetInt32());
        Assert.Equal(150, root.GetProperty("banana").GetInt32());
    }

    private static bool EnsureNativePathConfigured()
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
            return true;
        }

        var localPath = Path.Combine(AppContext.BaseDirectory, fileName);
        return File.Exists(localPath);
    }
}
