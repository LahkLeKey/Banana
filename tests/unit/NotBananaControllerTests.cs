using Banana.Api.Controllers;
using Banana.Api.Models;
using Banana.Api.Services;

using Microsoft.AspNetCore.Mvc;

using Xunit;

namespace Banana.UnitTests;

public sealed class NotBananaControllerTests
{
    [Fact]
    public void Classify_ReturnsOkPayload_FromService()
    {
        var controller = new NotBananaController(new FakeNotBananaService());
        var request = new NotBananaJunkRequest();

        var actionResult = controller.Classify(request);

        var ok = Assert.IsType<OkObjectResult>(actionResult.Result);
        var payload = Assert.IsType<NotBananaJunkResponse>(ok.Value);
        Assert.Equal("NOT_BANANA", payload.Classification);
        Assert.Equal(1, payload.ActorCount);
        Assert.Equal(1, payload.EntityCount);
    }

    private sealed class FakeNotBananaService : INotBananaService
    {
        public NotBananaJunkResponse Classify(NotBananaJunkRequest request)
        {
            return new NotBananaJunkResponse(
                "NOT_BANANA",
                0.1,
                0.9,
                0.95,
                1,
                1,
                [new NotBananaPolymorphicActor("worker", "w-1")],
                [new NotBananaPolymorphicEntity("crate", "c-1")],
                "ok");
        }
    }
}