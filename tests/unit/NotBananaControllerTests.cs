using Banana.Api.Controllers;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Mapping;
using Banana.Api.Pipeline.Results;
using Banana.Api.Tests.Unit.TestDoubles;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class NotBananaControllerTests
{
    [Fact]
    public void Junk_UsesMapperAndReturnsTypedResult()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyNotBananaJunkStatus = NativeStatusCode.Ok,
            ClassifyNotBananaJunkJson = "{\"label\":\"not_banana\",\"model\":\"binary\"}",
        };
        var mapper = new SpyNativeJsonMapper();
        var ctx = new PipelineContext();
        var controller = new NotBananaController(fake, mapper, ctx);

        var result = controller.Junk(new NotBananaController.JunkRequest("{\"text\":\"plastic engine junk\"}"));

        var ok = Assert.IsType<OkObjectResult>(result);
        var payload = Assert.IsType<NotBananaClassificationResult>(ok.Value);
        Assert.Equal("not_banana", payload.Label);
        Assert.Equal("binary", payload.Model);
        Assert.Equal(1, mapper.NotBananaDeserializeCalls);
        Assert.Equal("/not-banana/junk", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }

    [Fact]
    public void Junk_WhenMapperReturnsNull_ReturnsInvalidNativePayload()
    {
        var fake = new FakeNativeBananaClient
        {
            ClassifyNotBananaJunkStatus = NativeStatusCode.Ok,
            ClassifyNotBananaJunkJson = "{\"label\":\"not_banana\"}",
        };
        var mapper = new NullNativeJsonMapper();
        var ctx = new PipelineContext();
        var controller = new NotBananaController(fake, mapper, ctx);

        var result = controller.Junk(new NotBananaController.JunkRequest("{\"text\":\"junk\"}"));

        var objectResult = Assert.IsType<ObjectResult>(result);
        Assert.Equal(500, objectResult.StatusCode);
    }

    private sealed class SpyNativeJsonMapper : INativeJsonMapper
    {
        public int NotBananaDeserializeCalls { get; private set; }

        public T? Deserialize<T>(string json)
        {
            if (typeof(T) == typeof(NotBananaClassificationResult))
            {
                NotBananaDeserializeCalls++;
                return (T)(object)new NotBananaClassificationResult
                {
                    Label = "not_banana",
                    Model = "binary",
                    Confidence = 0.0,
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
