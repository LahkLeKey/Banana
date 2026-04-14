namespace Banana.Api.DataAccess;

/// <summary>
/// Executes one atomic database access operation for the current pipeline request.
/// </summary>
public interface IDataAccessPipelineClient
{
    /// <summary>
    /// Executes data access atomically and returns the raw result envelope.
    /// </summary>
    /// <param name="request">Input values required for query execution.</param>
    /// <returns>Raw result payload used by downstream steps.</returns>
    RawDbAccessResult Execute(DbAccessRequest request);
}
