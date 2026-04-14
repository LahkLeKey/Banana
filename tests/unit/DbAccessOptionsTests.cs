using CInteropSharp.Api.DataAccess;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class DbAccessOptionsTests
{
    [Fact]
    public void DefaultsToLegacyModeAndHasQueryTemplate()
    {
        var options = new DbAccessOptions();

        Assert.Equal(DbAccessMode.LegacyNative, options.Mode);
        Assert.Contains("@purchases", options.ManagedQuery, StringComparison.Ordinal);
    }
}
