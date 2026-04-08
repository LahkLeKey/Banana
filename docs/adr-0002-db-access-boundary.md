# ADR-0002: Database Access Pipeline Boundary

## Status

Accepted

## Context

PostgreSQL access may exist in legacy native C code.
The system now uses a composable API pipeline where each stage has explicit responsibility boundaries.
Database calls must not be mixed with compute concerns, and existing legacy SQL behavior must remain stable.

## Decision

1. Add a dedicated `DatabaseAccessStep` as a data-access stage in the request pipeline.
2. Treat the stage as atomic at the pipeline boundary:
   - input mapping -> query execution -> raw result -> context write-back.
3. Keep legacy-native mode as the default path using native C-backed behavior.
4. Do not reimplement legacy-native SQL logic in C# for the native mode.
5. Provide an optional managed PostgreSQL path via `Npgsql`, selected by configuration.
6. Keep legacy and modern modes isolated behind `IDataAccessPipelineClient`.

## Consequences

- Existing native SQL behavior remains preserved in default operation.
- Teams can modernize incrementally with a runtime mode switch and rollback path.
- Pipeline responsibilities are clearer: validation, data access, compute, enrichment, and audit are separate.
- Managed path improves maintainability but requires validation for parity and performance.
