using Banana.Api.DataAccess;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Steps;

using Xunit;

namespace Banana.UnitTests;

public sealed class DatabaseAccessStepTests
{
    [Fact]
    public void Order_IsExpected()
    {
        var step = new DatabaseAccessStep(new FakeDataAccessPipelineClient());
        Assert.Equal(150, step.Order);
    }

    [Fact]
    public void Execute_PopulatesRawDatabaseResultAndMetadata()
    {
        var context = new PipelineContext
        {
            Purchases = 5,
            Multiplier = 4
        };
        var client = new FakeDataAccessPipelineClient();
        var step = new DatabaseAccessStep(client);

        var result = step.Execute(context, static current => current);

        Assert.NotNull(result.DatabaseRawResult);
        Assert.Equal("native-dal", result.DatabaseRawResult!.Source);
        Assert.Equal(1, result.DatabaseRawResult.RowCount);
        Assert.Equal(DbAccessContract.BananaProfileProjection.ToString(), result.Metadata["db.contract"]);
        Assert.Equal("native-dal", result.Metadata["db.source"]);
        Assert.Equal(1, result.Metadata["db.rowCount"]);
    }

    private sealed class FakeDataAccessPipelineClient : IDataAccessPipelineClient
    {
        public RawDbAccessResult Execute(DbAccessRequest request)
        {
            return new RawDbAccessResult(
                "native-dal",
                $"{{\"purchases\":{request.Purchases},\"multiplier\":{request.Multiplier}}}",
                1);
        }
    }
}
