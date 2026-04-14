using Banana.Api.Controllers;
using Banana.Api.Models;
using Banana.Api.NativeInterop;
using Banana.Api.Services;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.UnitTests;

public sealed class BananaControllerTests
{
    [Fact]
    public void Get_ReturnsExpectedPayload()
    {
        var controller = new BananaController(new FakeBananaService());

        var actionResult = controller.Get(8, 4);

        var ok = Assert.IsType<OkObjectResult>(actionResult.Result);
        var payload = Assert.IsType<BananaResponse>(ok.Value);
        Assert.Equal(8, payload.Purchases);
        Assert.Equal(4, payload.Multiplier);
        Assert.Equal(32, payload.Banana);
        Assert.Equal("ok", payload.Message);
    }

    private sealed class FakeBananaService : IBananaService
    {
        public BananaResult Calculate(int purchases, int multiplier)
        {
            return new BananaResult(purchases, multiplier, purchases * multiplier, "ok");
        }
    }
}
