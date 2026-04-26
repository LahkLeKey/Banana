using System.Text.Json;

using Banana.Api.Controllers;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Tests.Unit.TestDoubles;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class BananaMlControllerTests
{
    [Fact]
    public void Regression_ReturnsScore_AndUpdatesPipelineContext()
    {
        var fake = new FakeNativeBananaClient
        {
            PredictRegressionStatus = NativeStatusCode.Ok,
            PredictRegressionValue = 0.8125,
        };
        var ctx = new PipelineContext();
        var controller = new BananaMlController(fake, ctx);

        var result = controller.Regression(new BananaMlController.MlRequest("{\"text\":\"ripe banana bunch\"}"));

        var ok = Assert.IsType<OkObjectResult>(result);
        var payload = SerializeToElement(ok.Value);
        Assert.Equal(0.8125, payload.GetProperty("score").GetDouble(), 4);
        Assert.Equal("/ml/regression", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }

    [Fact]
    public void Binary_PassesNativeMetricsJsonThroughManagedEndpoint()
    {
        const string nativeJson = "{\"label\":\"banana\",\"confidence\":0.88,\"jaccard\":0.5,\"confusion_matrix\":{\"tp\":1,\"fp\":1,\"fn\":0,\"tn\":0}}";
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
            ClassifyBananaBinaryJson = nativeJson,
        };
        var ctx = new PipelineContext();
        var controller = new BananaMlController(fake, ctx);

        var result = controller.Binary(new BananaMlController.MlRequest("{\"text\":\"banana bread crate\"}"));

        var ok = Assert.IsType<OkObjectResult>(result);
        var payload = SerializeToElement(ok.Value);
        Assert.Equal("banana", payload.GetProperty("label").GetString());
        Assert.True(payload.GetProperty("confidence").GetDouble() > 0.0);
        Assert.True(payload.TryGetProperty("jaccard", out _));
        Assert.True(payload.TryGetProperty("confusion_matrix", out _));
        Assert.Equal("/ml/binary", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }

    [Fact]
    public void Transformer_MapsInvalidArgument_ToBadRequest()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaTransformerStatus = NativeStatusCode.InvalidArgument,
        };
        var ctx = new PipelineContext();
        var controller = new BananaMlController(fake, ctx);

        var result = controller.Transformer(new BananaMlController.MlRequest("{}"));

        Assert.IsType<BadRequestObjectResult>(result);
        Assert.Equal("/ml/transformer", ctx.Route);
        Assert.Equal(NativeStatusCode.InvalidArgument, ctx.LastStatus);
    }

    [Fact]
    public void NotBananaController_PassesNativeMetricsJsonThroughManagedEndpoint()
    {
        const string nativeJson = "{\"model\":\"binary\",\"label\":\"not_banana\",\"jaccard\":1.0,\"confusion_matrix\":{\"tp\":0,\"fp\":0,\"fn\":0,\"tn\":1}}";
        var fake = new FakeNativeBananaClient
        {
            ClassifyNotBananaJunkStatus = NativeStatusCode.Ok,
            ClassifyNotBananaJunkJson = nativeJson,
        };
        var ctx = new PipelineContext();
        var controller = new NotBananaController(fake, ctx);

        var result = controller.Junk(new NotBananaController.JunkRequest("{\"text\":\"plastic engine junk\"}"));

        var ok = Assert.IsType<OkObjectResult>(result);
        var payload = SerializeToElement(ok.Value);
        Assert.Equal("not_banana", payload.GetProperty("label").GetString());
        Assert.Equal("binary", payload.GetProperty("model").GetString());
        Assert.True(payload.TryGetProperty("jaccard", out _));
        Assert.True(payload.TryGetProperty("confusion_matrix", out _));
        Assert.Equal("/not-banana/junk", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }

    private static JsonElement SerializeToElement(object? value)
    {
        using var document = JsonDocument.Parse(JsonSerializer.Serialize(value));
        return document.RootElement.Clone();
    }
}