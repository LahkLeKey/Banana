using CInteropSharp.Api.NativeInterop;
using CInteropSharp.Api.Services;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class PointsServiceTests
{
    [Fact]
    public void Calculate_DelegatesToNativeClient()
    {
        var fake = new FakeNativePointsClient(new PointsResult(10, 2, 150, "ok"));
        var service = new PointsService(fake);

        var result = service.Calculate(10, 2);

        Assert.Equal(150, result.Points);
        Assert.Equal("ok", result.Message);
    }

    [Fact]
    public void Calculate_WithNegativeValues_ThrowsClientInputException()
    {
        var fake = new FakeNativePointsClient(new PointsResult(0, 0, 0, "unused"));
        var service = new PointsService(fake);

        Assert.Throws<ClientInputException>(() => service.Calculate(-1, 2));
    }

    private sealed class FakeNativePointsClient : INativePointsClient
    {
        private readonly PointsResult _result;

        public FakeNativePointsClient(PointsResult result)
        {
            _result = result;
        }

        public PointsResult Calculate(int purchases, int multiplier)
        {
            return _result;
        }
    }
}
