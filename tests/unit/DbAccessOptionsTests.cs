using Banana.Api.DataAccess;

using Xunit;

namespace Banana.UnitTests;

public sealed class DbAccessOptionsTests
{
    [Fact]
    public void DefaultsToNativeDalModeAndHasBananaProfileQuery()
    {
        var options = new DbAccessOptions();

        Assert.Equal(DbAccessMode.NativeDal, options.Mode);
        Assert.Contains("banana_yield", options.BananaProfileQuery, StringComparison.OrdinalIgnoreCase);
        Assert.Equal(options.BananaProfileQuery, options.ManagedQuery);
    }

    [Fact]
    public void GetManagedQuery_ResolvesBananaProfileProjection()
    {
        var options = new DbAccessOptions();

        var query = options.GetManagedQuery(DbAccessContract.BananaProfileProjection);

        Assert.Equal(options.BananaProfileQuery, query);
    }
}
