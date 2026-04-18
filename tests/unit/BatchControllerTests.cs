using Banana.Api.Controllers;
using Banana.Api.Models;
using Banana.Api.Services;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.UnitTests;

public sealed class BatchControllerTests
{
    [Fact]
    public void Create_ReturnsExpectedPayload()
    {
        var controller = new BatchController(new FakeBatchService());

        var actionResult = controller.Create(new BananaBatchCreateRequest("batch-1", "farm-1", 13.2, 2.5, 3));

        var ok = Assert.IsType<OkObjectResult>(actionResult.Result);
        var payload = Assert.IsType<BananaBatchResponse>(ok.Value);
        Assert.Equal("batch-1", payload.BatchId);
        Assert.Equal("PACKED", payload.ExportStatus);
    }

    [Fact]
    public void GetStatus_ReturnsExpectedPayload()
    {
        var controller = new BatchController(new FakeBatchService());

        var actionResult = controller.GetStatus("batch-1");

        var ok = Assert.IsType<OkObjectResult>(actionResult.Result);
        var payload = Assert.IsType<BananaBatchResponse>(ok.Value);
        Assert.Equal("batch-1", payload.BatchId);
    }

    private sealed class FakeBatchService : IBatchService
    {
        public BananaBatchResponse Create(BananaBatchCreateRequest request)
        {
            return new BananaBatchResponse(request.BatchId, request.OriginFarm, "PACKED", request.StorageTempC, request.EthyleneExposure, request.EstimatedShelfLifeDays);
        }

        public BananaBatchResponse GetStatus(string batchId)
        {
            return new BananaBatchResponse(batchId, "farm-1", "PACKED", 13.2, 2.5, 3);
        }
    }
}