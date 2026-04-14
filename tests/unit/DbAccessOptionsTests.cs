using Banana.Api.DataAccess;

using Xunit;

namespace Banana.UnitTests;

public sealed class DbAccessOptionsTests
{
    [Fact]
    public void DefaultsToNativeDalModeAndHasQueryTemplate()
    {
        var options = new DbAccessOptions();

        Assert.Equal(DbAccessMode.NativeDal, options.Mode);
        Assert.Contains("@purchases", options.ManagedQuery, StringComparison.Ordinal);
    }
}
