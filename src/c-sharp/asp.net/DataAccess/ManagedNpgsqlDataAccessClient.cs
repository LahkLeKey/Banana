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

    /// <summary>
    /// Initializes a new instance of the <see cref="ManagedNpgsqlDataAccessClient"/> class.
    /// </summary>
    public ManagedNpgsqlDataAccessClient(IConfiguration configuration, IOptions<DbAccessOptions> options)
    {
        _configuration = configuration;
        _options = options.Value;
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
