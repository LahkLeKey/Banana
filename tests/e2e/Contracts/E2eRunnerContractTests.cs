using System.Text.Json;

using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.E2eTests.Contracts;

public sealed class E2eRunnerContractTests
{
    [Fact]
    public void DbNotConfiguredContractPayloadIsStableForE2eAssertions()
    {
        var result = StatusMapping.ToActionResult(NativeStatusCode.DbNotConfigured);

        var objectResult = Assert.IsType<ObjectResult>(result);
        Assert.Equal(503, objectResult.StatusCode);

        var payload = SerializeToElement(objectResult.Value);
        Assert.Equal("config_missing", payload.GetProperty("error").GetString());
        Assert.Equal("Set BANANA_PG_CONNECTION.", payload.GetProperty("remediation").GetString());
    }

    [Fact]
    public void NativeUnavailableContractPayloadIsStableForE2eAssertions()
    {
        var result = StatusMapping.ToActionResult(NativeStatusCode.NativeUnavailable);

        var objectResult = Assert.IsType<ObjectResult>(result);
        Assert.Equal(503, objectResult.StatusCode);

        var payload = SerializeToElement(objectResult.Value);
        Assert.Equal("native_unavailable", payload.GetProperty("error").GetString());
        Assert.Equal("Set BANANA_NATIVE_PATH to a valid native library path.", payload.GetProperty("remediation").GetString());
    }

    private static JsonElement SerializeToElement(object? value)
    {
        using var document = JsonDocument.Parse(JsonSerializer.Serialize(value));
        return document.RootElement.Clone();
    }
}
