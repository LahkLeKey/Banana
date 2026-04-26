using Banana.Api.Controllers;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Tests.Unit.TestDoubles;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.Api.Tests.Unit;

public sealed class HarvestControllerTests
{
    [Fact]
    public void Create_ReturnsTypedHarvestPayload_AndUpdatesContext()
    {
        var fake = new FakeNativeBananaClient
        {
            CreateHarvestBatchStatusCode = NativeStatusCode.Ok,
            CreateHarvestBatchJson = "{\"harvest_batch_id\":\"harvest-0001\",\"status\":\"created\",\"bunch_count\":0}",
        };
        var ctx = new PipelineContext();
        var controller = new HarvestController(fake, ctx);

        var result = controller.Create(new HarvestController.InputJsonRequest("{}"));

        var ok = Assert.IsType<OkObjectResult>(result);
        var payload = Assert.IsType<HarvestController.HarvestBatchResult>(ok.Value);
        Assert.Equal("harvest-0001", payload.HarvestBatchId);
        Assert.Equal("created", payload.Status);
        Assert.Equal(0, payload.BunchCount);
        Assert.Equal("/harvest/create", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }

    [Fact]
    public void AddBunch_WhenBatchMissing_MapsToNotFound()
    {
        var fake = new FakeNativeBananaClient
        {
            AddBunchToHarvestBatchStatusCode = NativeStatusCode.NotFound,
        };
        var ctx = new PipelineContext();
        var controller = new HarvestController(fake, ctx);

        var result = controller.AddBunch("harvest-9999", new HarvestController.InputJsonRequest("{\"count\":\"bunch\"}"));

        Assert.IsType<NotFoundObjectResult>(result);
        Assert.Equal("/harvest/harvest-9999/bunches", ctx.Route);
        Assert.Equal(NativeStatusCode.NotFound, ctx.LastStatus);
    }

    [Fact]
    public void Status_WhenNativePayloadInvalid_ReturnsInvalidNativePayload()
    {
        var fake = new FakeNativeBananaClient
        {
            GetHarvestBatchStatusCode = NativeStatusCode.Ok,
            GetHarvestBatchStatusJson = "{\"status\":true}",
        };
        var ctx = new PipelineContext();
        var controller = new HarvestController(fake, ctx);

        var result = controller.Status("harvest-0001");

        var objectResult = Assert.IsType<ObjectResult>(result);
        Assert.Equal(500, objectResult.StatusCode);
        Assert.Equal("/harvest/harvest-0001/status", ctx.Route);
        Assert.Equal(NativeStatusCode.Ok, ctx.LastStatus);
    }
}
