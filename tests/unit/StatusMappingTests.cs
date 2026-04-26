using System.Text.Json;

using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class StatusMappingTests
{
    [Fact]
    public void ToActionResult_NativeUnavailable_Returns503WithRemediation()
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
