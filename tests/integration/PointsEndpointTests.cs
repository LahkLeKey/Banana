using System.Net;
using System.Runtime.InteropServices;

using CInteropSharp.Api.NativeInterop;

using Microsoft.AspNetCore.Mvc.Testing;

using Xunit;

namespace CInteropSharp.IntegrationTests;

public sealed class PointsEndpointTests : IClassFixture<WebApplicationFactory<Program>>
{
    private readonly WebApplicationFactory<Program> _factory;

    public PointsEndpointTests(WebApplicationFactory<Program> factory)
    {
        _factory = factory;
    }

    [Fact]
    public async Task GetPoints_ReturnsOk_WhenNativeLibraryIsAvailable()
    {
        if (!EnsureNativePathConfigured())
        {
            return;
        }

        using var client = _factory.CreateClient();
        var response = await client.GetAsync("/points?purchases=10&multiplier=2");

        Assert.Equal(HttpStatusCode.OK, response.StatusCode);
    }

    private static bool EnsureNativePathConfigured()
    {
        var root = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "../../../../"));
        var candidate = Path.Combine(root, "build", "native", "bin", "Release");

        if (Directory.Exists(candidate))
        {
            Environment.SetEnvironmentVariable("CINTEROP_NATIVE_PATH", candidate);
            return true;
        }

        var localName = NativeLibraryResolver.GetPlatformLibraryName();
        var localPath = Path.Combine(AppContext.BaseDirectory, localName);
        return File.Exists(localPath);
    }
}
