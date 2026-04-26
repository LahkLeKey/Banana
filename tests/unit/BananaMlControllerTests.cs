using System.Text.Json;

using Banana.Api.Controllers;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Mapping;
using Banana.Api.Pipeline.Results;
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
        var mapper = new SpyNativeJsonMapper();
        var ctx = new PipelineContext();
        var controller = new BananaMlController(fake, mapper, ctx, new PipelineRunner<PipelineContext>(Array.Empty<IPipelineStep<PipelineContext>>()));

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
        var mapper = new SpyNativeJsonMapper();
        var ctx = new PipelineContext();
        var controller = new BananaMlController(fake, mapper, ctx, new PipelineRunner<PipelineContext>(Array.Empty<IPipelineStep<PipelineContext>>()));

        var result = controller.Binary(new BananaMlController.MlRequest("{\"text\":\"banana bread crate\"}"));

        var ok = Assert.IsType<OkObjectResult>(result);
        var payload = Assert.IsType<BinaryClassificationResult>(ok.Value);
        Assert.Equal("banana", payload.Label);
        Assert.True(payload.Confidence > 0.0);
        Assert.True(payload.Metrics.ContainsKey("jaccard"));
        Assert.True(payload.Metrics.ContainsKey("confusion_matrix"));
        Assert.Equal(1, mapper.BinaryDeserializeCalls);
        Assert.Equal("/ml/binary", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }

    [Fact]
    public void Binary_WhenMapperReturnsNull_ReturnsInvalidNativePayload()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaBinaryStatus = NativeStatusCode.Ok,
            ClassifyBananaBinaryJson = "{\"label\":\"banana\"}",
        };
        var mapper = new NullNativeJsonMapper();
        var ctx = new PipelineContext();
        var controller = new BananaMlController(fake, mapper, ctx, new PipelineRunner<PipelineContext>(Array.Empty<IPipelineStep<PipelineContext>>()));

        var result = controller.Binary(new BananaMlController.MlRequest("{\"text\":\"banana\"}"));

        var objectResult = Assert.IsType<ObjectResult>(result);
        Assert.Equal(500, objectResult.StatusCode);
    }

    [Fact]
    public void Transformer_MapsInvalidArgument_ToBadRequest()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyBananaTransformerStatus = NativeStatusCode.InvalidArgument,
        };
        var mapper = new SpyNativeJsonMapper();
        var ctx = new PipelineContext();
        var controller = new BananaMlController(fake, mapper, ctx, new PipelineRunner<PipelineContext>(Array.Empty<IPipelineStep<PipelineContext>>()));

        var result = controller.Transformer(new BananaMlController.MlRequest("{}"));

        Assert.IsType<BadRequestObjectResult>(result);
        Assert.Equal("/ml/transformer", ctx.Route);
        Assert.Equal(NativeStatusCode.InvalidArgument, ctx.LastStatus);
    }

    private static JsonElement SerializeToElement(object? value)
    {
        using var document = JsonDocument.Parse(JsonSerializer.Serialize(value));
        return document.RootElement.Clone();
    }

    private sealed class SpyNativeJsonMapper : INativeJsonMapper
    {
        public int BinaryDeserializeCalls { get; private set; }

        public T? Deserialize<T>(string json)
        {
            if (typeof(T) == typeof(BinaryClassificationResult))
            {
                BinaryDeserializeCalls++;
                return (T)(object)new BinaryClassificationResult
                {
                    Label = "banana",
                    Confidence = 0.88,
                    Model = "binary",
                    Metrics = new Dictionary<string, JsonElement>
                    {
                        ["jaccard"] = JsonDocument.Parse("0.5").RootElement.Clone(),
                        ["confusion_matrix"] = JsonDocument.Parse("{\"tp\":1,\"fp\":1,\"fn\":0,\"tn\":0}").RootElement.Clone(),
                    },
                };
            }

            return default;
        }
    }

    private sealed class NullNativeJsonMapper : INativeJsonMapper
    {
        public T? Deserialize<T>(string json) => default;
    }
}