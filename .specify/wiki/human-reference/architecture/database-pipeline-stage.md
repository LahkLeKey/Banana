<!-- breadcrumb: Architecture > Database Pipeline Stage -->

# Database Pipeline Stage

> [Home](../Home.md) › [Architecture](README.md) › Database Pipeline Stage

Related pages: [How A Request Works](../getting-started/how-a-request-works.md), [Build, Run, and Test Commands](../getting-started/build-run-test-commands.md)

This page explains the `DatabaseAccessStep` used by `GET /banana`.

## Why There Is A Separate DB Step

Database work is kept separate so it is easier to test, debug, and replace.

The DB step does one job:

- take `purchases` and `multiplier` from `PipelineContext`,
- execute a named DAL contract (`BananaProfileProjection`),
- return DB metadata and payload to the pipeline context.

## Two Database Modes

- `NativeDal` mode:
   Uses wrapper export `banana_db_query_banana_profile` and native DAL code in `src/native/core/dal`.
- `ManagedNpgsql` mode:
   Uses managed Npgsql execution in `ManagedNpgsqlDataAccessClient`.

Both modes use the same interface, so the rest of the request flow stays the same.

## How To Choose A Mode

Mode is selected by `DbAccess:Mode` in configuration (`appsettings.*` or env overrides).

`Program.cs` resolves one implementation for `IDataAccessPipelineClient` based on that mode.

## Contract And Metadata

`DbAccessRequest` includes:

- `Purchases`
- `Multiplier`
- `Contract` (default `BananaProfileProjection`)

`DatabaseAccessStep` writes metadata used downstream and surfaced by `BananaController` headers:

- `db.contract` -> `X-Banana-Db-Contract`
- `db.source` -> `X-Banana-Db-Source`
- `db.rowCount` -> `X-Banana-Db-RowCount`

## Safe Change Workflow

If you are changing DB behavior:

1. Decide whether the change belongs to `NativeDal`, `ManagedNpgsql`, or both.
2. Keep logic inside the relevant data-access client.
3. Keep step wiring in `DatabaseAccessStep` and `Program.cs` only.
4. Add unit tests for the changed client.
5. Add integration tests that assert response status plus DB metadata headers.

## Common Failure Checks

- Mode looks wrong: verify `DbAccess:Mode` and option binding.
- Native DB errors: verify `BANANA_PG_CONNECTION` and PostgreSQL reachability.
- Native library load errors: verify `BANANA_NATIVE_PATH`.
- Managed DB errors: verify `ConnectionStrings:DefaultConnection` and `DbAccess:BananaProfileQuery`.
- API returns 503: check `DatabaseAccessException` in logs (mapped by middleware).
