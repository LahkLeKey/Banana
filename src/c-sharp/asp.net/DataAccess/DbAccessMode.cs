namespace Banana.Api.DataAccess;

/// <summary>
/// Selects which implementation backs the database pipeline stage.
/// </summary>
public enum DbAccessMode
{
    /// <summary>
    /// Uses native C interop as the source of SQL behavior.
    /// </summary>
    NativeDal = 0,

    /// <summary>
    /// Uses managed PostgreSQL access via Npgsql.
    /// </summary>
    ManagedNpgsql = 1
}
