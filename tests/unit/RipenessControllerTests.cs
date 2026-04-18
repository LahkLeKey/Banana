using Banana.Api.Controllers;
using Banana.Api.Models;
using Banana.Api.Services;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.UnitTests;

public sealed class RipenessControllerTests
{
    [Fact]
    public void Predict_ReturnsExpectedPayload()
    {
        var controller = new RipenessController(new FakeRipenessService());

        var actionResult = controller.Predict(new BananaRipenessRequest("batch-1", [12.5, 13.0], 4, 1.0, 0.1, 13.0));

        var ok = Assert.IsType<OkObjectResult>(actionResult.Result);
        var payload = Assert.IsType<BananaRipenessResponse>(ok.Value);
        Assert.Equal("batch-1", payload.BatchId);
        Assert.Equal("YELLOW", payload.PredictedStage);
        Assert.Equal(48, payload.ShelfLifeHours);
    }

    private sealed class FakeRipenessService : IRipenessService
    {
        public BananaRipenessResponse Predict(BananaRipenessRequest request)
        {
            return new BananaRipenessResponse(request.BatchId, "YELLOW", 48, 180.0, 0.3, 0.15);
        }
    }
}