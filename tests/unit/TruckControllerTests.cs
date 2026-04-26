using Banana.Api.Controllers;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Tests.Unit.TestDoubles;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class TruckControllerTests
{
    [Fact]
    public void Register_ReturnsTypedTruckPayload_AndUpdatesContext()
    {
        var fake = new FakeNativeBananaClient
        {
            RegisterTruckStatusCode = NativeStatusCode.Ok,
            RegisterTruckJson = "{\"truck_id\":\"truck-0001\",\"status\":\"registered\",\"location\":\"warehouse\",\"container_count\":0}",
        };
        var ctx = new PipelineContext();
        var controller = new TruckController(fake, ctx);

        var result = controller.Register(new TruckController.InputJsonRequest("{\"destination\":\"warehouse\"}"));

        var ok = Assert.IsType<OkObjectResult>(result);
        var payload = Assert.IsType<TruckController.TruckResult>(ok.Value);
        Assert.Equal("truck-0001", payload.TruckId);
        Assert.Equal("registered", payload.Status);
        Assert.Equal("warehouse", payload.Location);
        Assert.Equal(0, payload.ContainerCount);
        Assert.Equal("/trucks/register", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }

    [Fact]
    public void Load_WhenTruckMissing_MapsToNotFound()
    {
        var fake = new FakeNativeBananaClient
        {
            LoadTruckContainerStatusCode = NativeStatusCode.NotFound,
        };
        var ctx = new PipelineContext();
        var controller = new TruckController(fake, ctx);

        var result = controller.Load("truck-9999", new TruckController.InputJsonRequest("{\"container\":\"c1\"}"));

        Assert.IsType<NotFoundObjectResult>(result);
        Assert.Equal("/trucks/truck-9999/containers/load", ctx.Route);
        Assert.Equal(NativeStatusCode.NotFound, ctx.LastStatus);
    }

    [Fact]
    public void Status_WhenNativePayloadInvalid_ReturnsInvalidNativePayload()
    {
        var fake = new FakeNativeBananaClient
        {
            GetTruckStatusCode = NativeStatusCode.Ok,
            GetTruckStatusJson = "{\"status\":true}",
        };
        var ctx = new PipelineContext();
        var controller = new TruckController(fake, ctx);

        var result = controller.Status("truck-0001");

        var objectResult = Assert.IsType<ObjectResult>(result);
        Assert.Equal(500, objectResult.StatusCode);
        Assert.Equal("/trucks/truck-0001/status", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }
}
