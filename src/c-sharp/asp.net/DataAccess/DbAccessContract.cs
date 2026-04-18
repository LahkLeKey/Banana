namespace Banana.Api.DataAccess;

/// <summary>
/// Named DAL contracts that the managed pipeline can execute across native and managed DB implementations.
/// </summary>
public enum DbAccessContract
{
    /// <summary>
    /// Banana profile projection used by the current DB stage.
    /// </summary>
    BananaProfileProjection = 0
}