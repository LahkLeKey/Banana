using System.Text.Json;

using Banana.Api.Controllers;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Tests.Unit.TestDoubles;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class RipenessControllerTests
{
    [Fact]
    public async Task Predict_WhenNativeUnavailable_Returns503NativeUnavailablePayload()
    {
        var fakeNative = new FakeNativeBananaClient
        {
            PredictBananaRipenessStatus = NativeStatusCode.NativeUnavailable,
        };
        var context = new PipelineContext();
        var runner = new PipelineRunner<PipelineContext>(Array.Empty<IPipelineStep<PipelineContext>>());
        var controller = new RipenessController(fakeNative, context, runner);

        var result = await controller.Predict(new RipenessController.RipenessRequest("{\"text\":\"ripe\"}"), CancellationToken.None);

        var objectResult = Assert.IsType<ObjectResult>(result);
        Assert.Equal(503, objectResult.StatusCode);

        var payload = SerializeToElement(objectResult.Value);
        Assert.Equal("native_unavailable", payload.GetProperty("error").GetString());
        Assert.Equal("Set BANANA_NATIVE_PATH to a valid native library path.", payload.GetProperty("remediation").GetString());
        Assert.Equal("/ripeness/predict", context.Route);
        Assert.Equal(NativeStatusCode.NativeUnavailable, context.LastStatus);
    }

    private static JsonElement SerializeToElement(object? value)
    {
        using var document = JsonDocument.Parse(JsonSerializer.Serialize(value));
        return document.RootElement.Clone();
    }
}
