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
    /// Gets or sets the SQL query template used by the managed Npgsql banana profile projection.
    /// </summary>
    public string BananaProfileQuery { get; set; } =
        "with banana_input as (" +
        " select @purchases::int as purchases, @multiplier::int as multiplier" +
        "), banana_yield as (" +
        " select purchases, multiplier, purchases * 10 as base_banana," +
        " case when purchases >= 10 then multiplier * 25 else 0 end as bonus_banana" +
        " from banana_input" +
        "), ripeness_profile as (" +
        " select purchases, multiplier, (base_banana + bonus_banana) as banana," +
        " 'Cavendish'::text as cultivar," +
        " ((purchases::double precision * 18.0) * 0.85) + (multiplier::double precision * 4.0) as ripening_index" +
        " from banana_yield" +
        ")" +
        " select purchases, multiplier, banana, cultivar," +
        " case" +
        "   when ripening_index < 80.0 then 'GREEN'" +
        "   when ripening_index < 140.0 then 'BREAKING'" +
        "   when ripening_index < 220.0 then 'YELLOW'" +
        "   when ripening_index < 300.0 then 'SPOTTED'" +
        "   when ripening_index < 360.0 then 'OVERRIPE'" +
        "   else 'BIODEGRADATION'" +
        " end as predicted_stage," +
        " greatest(0, floor(360.0 - ripening_index))::int as shelf_life_hours," +
        " least(1.0, greatest(0.0, ripening_index / 360.0)) as spoilage_probability" +
        " from ripeness_profile";

    /// <summary>
    /// Gets or sets the legacy managed query alias for backward-compatible configuration binding.
    /// </summary>
    public string ManagedQuery
    {
        get => BananaProfileQuery;
        set => BananaProfileQuery = value;
    }

    /// <summary>
    /// Resolves the managed SQL template for a named DAL contract.
    /// </summary>
    /// <param name="contract">Contract to resolve.</param>
    /// <returns>SQL text used by the managed Npgsql client.</returns>
    /// <exception cref="InvalidOperationException">Thrown when the contract is not supported.</exception>
    public string GetManagedQuery(DbAccessContract contract)
    {
        return contract switch
        {
            DbAccessContract.BananaProfileProjection => BananaProfileQuery,
            _ => throw new InvalidOperationException($"Unsupported DB access contract: {contract}")
        };
    }
}
