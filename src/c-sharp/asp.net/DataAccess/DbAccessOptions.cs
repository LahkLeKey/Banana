namespace Banana.Api.DataAccess;

/// <summary>
/// Configuration for selecting and parameterizing data access mode.
/// </summary>
public sealed class DbAccessOptions
{
    /// <summary>
    /// Configuration section name.
    /// </summary>
    public const string SectionName = "DbAccess";

    /// <summary>
    /// Gets or sets the active data access mode.
    /// </summary>
    public DbAccessMode Mode { get; set; } = DbAccessMode.NativeDal;

    /// <summary>
    /// Gets or sets the SQL query template used by the managed Npgsql mode.
    /// </summary>
    public string ManagedQuery { get; set; } =
        "select @purchases::int as purchases, @multiplier::int as multiplier";
}
