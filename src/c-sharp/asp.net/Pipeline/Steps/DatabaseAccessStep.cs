using Banana.Api.DataAccess;

namespace Banana.Api.Pipeline.Steps;

/// <summary>
/// Executes a single atomic database stage and writes the raw result into pipeline context.
/// </summary>
public sealed class DatabaseAccessStep : IPipelineStep<PipelineContext>
{
    private readonly IDataAccessPipelineClient _dataAccessClient;

    /// <summary>
    /// Initializes a new instance of the <see cref="DatabaseAccessStep"/> class.
    /// </summary>
    /// <param name="dataAccessClient">Data access client selected by configuration mode.</param>
    public DatabaseAccessStep(IDataAccessPipelineClient dataAccessClient)
    {
        _dataAccessClient = dataAccessClient;
    }

    /// <inheritdoc />
    public int Order => 150;

    /// <inheritdoc />
    public PipelineContext Execute(PipelineContext input, Func<PipelineContext, PipelineContext> next)
    {
        var request = new DbAccessRequest(input.Purchases, input.Multiplier);
        var result = _dataAccessClient.Execute(request);

        input.DatabaseRawResult = result;
        input.Metadata["db.contract"] = request.Contract.ToString();
        input.Metadata["db.source"] = result.Source;
        input.Metadata["db.rowCount"] = result.RowCount;

        return next(input);
    }
}
