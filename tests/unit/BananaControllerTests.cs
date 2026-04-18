using Banana.Api.Controllers;
using Banana.Api.Models;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.UnitTests;

public sealed class BananaControllerTests
{
    [Fact]
    public void Get_ReturnsExpectedPayload()
    {
        var controller = new BananaController(new FakeBananaService());
        controller.ControllerContext = new ControllerContext
        {
            HttpContext = new DefaultHttpContext()
        };

        var actionResult = controller.Get(8, 4);

        var ok = Assert.IsType<OkObjectResult>(actionResult.Result);
        var payload = Assert.IsType<BananaResponse>(ok.Value);
        Assert.Equal(8, payload.Purchases);
        Assert.Equal(4, payload.Multiplier);
        Assert.Equal(32, payload.Banana);
        Assert.Equal("ok", payload.Message);
        Assert.Equal("BananaProfileProjection", controller.Response.Headers["X-Banana-Db-Contract"]);
        Assert.Equal("native-dal", controller.Response.Headers["X-Banana-Db-Source"]);
        Assert.Equal("1", controller.Response.Headers["X-Banana-Db-RowCount"]);
    }

    private sealed class FakeBananaService : IBananaService
    {
        public BananaCalculationResult Calculate(int purchases, int multiplier)
        {
            return new BananaCalculationResult(
                new BananaResult(purchases, multiplier, purchases * multiplier, "ok"),
                new Dictionary<string, object>
                {
                    ["db.contract"] = "BananaProfileProjection",
                    ["db.source"] = "native-dal",
                    ["db.rowCount"] = 1
                });
        }
    }
}
