using CInteropSharp.Api.Models;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class BananaResponseTests
{
    [Fact]
    public void ExposesRecordProperties()
    {
        var response = new BananaResponse(10, 2, 150, "done");

        Assert.Equal(10, response.Purchases);
        Assert.Equal(2, response.Multiplier);
        Assert.Equal(150, response.Banana);
        Assert.Equal("done", response.Message);
    }
}
