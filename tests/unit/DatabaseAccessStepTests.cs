using CInteropSharp.Api.DataAccess;
using CInteropSharp.Api.Pipeline;
using CInteropSharp.Api.Pipeline.Steps;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class DatabaseAccessStepTests
{
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
        Assert.Equal("legacy-native", result.DatabaseRawResult!.Source);
        Assert.Equal(1, result.DatabaseRawResult.RowCount);
        Assert.Equal("legacy-native", result.Metadata["db.source"]);
        Assert.Equal(1, result.Metadata["db.rowCount"]);
    }

    private sealed class FakeDataAccessPipelineClient : IDataAccessPipelineClient
    {
        public RawDbAccessResult Execute(DbAccessRequest request)
        {
            return new RawDbAccessResult(
                "legacy-native",
                $"{{\"purchases\":{request.Purchases},\"multiplier\":{request.Multiplier}}}",
                1);
        }
    }
}
