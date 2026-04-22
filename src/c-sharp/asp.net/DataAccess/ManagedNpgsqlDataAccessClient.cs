using System.Diagnostics.CodeAnalysis;
using System.Text.Json;

using Microsoft.Extensions.Options;

using Npgsql;

namespace Banana.Api.DataAccess;

/// <summary>
/// Optional managed implementation of the DB stage using Npgsql.
/// </summary>
public sealed class ManagedNpgsqlDataAccessClient : IDataAccessPipelineClient
{
    private readonly IConfiguration _configuration;
    private readonly DbAccessOptions _options;
    private readonly Func<string, string, DbAccessRequest, Dictionary<string, object?>> _executeQuery;

    /// <summary>
    /// Initializes a new instance of the <see cref="ManagedNpgsqlDataAccessClient"/> class.
    /// </summary>
    public ManagedNpgsqlDataAccessClient(IConfiguration configuration, IOptions<DbAccessOptions> options)
        : this(configuration, options, ExecuteQuery)
    {
    }

    internal ManagedNpgsqlDataAccessClient(
        IConfiguration configuration,
        IOptions<DbAccessOptions> options,
        Func<string, string, DbAccessRequest, Dictionary<string, object?>> executeQuery)
    {
        _configuration = configuration;
        _options = options.Value;
        _executeQuery = executeQuery;
    }

    /// <inheritdoc />
    public RawDbAccessResult Execute(DbAccessRequest request)
    {
        var query = ResolveQuery(request.Contract);
        var connectionString = _configuration.GetConnectionString("PostgreSQL");
        if (string.IsNullOrWhiteSpace(connectionString))
        {
            throw new DatabaseAccessException("Managed PostgreSQL mode requires a configured connection string.");
        }

        try
        {
            var row = _executeQuery(connectionString, query, request);
            var payload = JsonSerializer.Serialize(row);
            return new RawDbAccessResult("managed-npgsql", payload, 1);
        }
        catch (DatabaseAccessException)
        {
            throw;
        }
        catch (NpgsqlException ex)
        {
            throw new DatabaseAccessException("Managed PostgreSQL query failed.", ex);
        }
        catch (Exception ex)
        {
            throw new DatabaseAccessException("Managed PostgreSQL connection failed.", ex);
        }
    }

    [ExcludeFromCodeCoverage(Justification = "Exercised through PostgreSQL-backed integration runs.")]
    private static Dictionary<string, object?> ExecuteQuery(string connectionString, string query, DbAccessRequest request)
    {
        using var connection = new NpgsqlConnection(connectionString);
        connection.Open();

        using var command = connection.CreateCommand();
        command.CommandText = query;
        command.Parameters.AddWithValue("purchases", request.Purchases);
        command.Parameters.AddWithValue("multiplier", request.Multiplier);

        using var reader = command.ExecuteReader();
        if (!reader.Read())
        {
            throw new DatabaseAccessException($"Managed PostgreSQL query for contract '{request.Contract}' returned no rows.");
        }

        var row = ReadRow(reader);
        if (reader.Read())
        {
            throw new DatabaseAccessException($"Managed PostgreSQL query for contract '{request.Contract}' returned more than one row.");
        }

        return row;
    }

    private string ResolveQuery(DbAccessContract contract)
    {
        try
        {
            return _options.GetManagedQuery(contract);
        }
        catch (InvalidOperationException ex)
        {
            throw new DatabaseAccessException($"Managed PostgreSQL mode does not support contract '{contract}'.", ex);
        }
    }

    [ExcludeFromCodeCoverage(Justification = "Exercised through PostgreSQL-backed integration runs.")]
    private static Dictionary<string, object?> ReadRow(NpgsqlDataReader reader)
    {
        var row = new Dictionary<string, object?>(StringComparer.Ordinal);
        for (var index = 0; index < reader.FieldCount; index++)
        {
            var value = reader.IsDBNull(index) ? null : reader.GetValue(index);
            row[reader.GetName(index)] = value;
        }

        return row;
    }
}
