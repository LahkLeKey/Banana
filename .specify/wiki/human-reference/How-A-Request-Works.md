# How A Request Works

> Read the [Wiki Home](Home.md) for more details.

Related pages: [Why We Use A Wrapper](Why-We-Use-A-Wrapper.md), [How The Database Step Works](How-The-Database-Step-Works.md), [Architecture Diagrams](Architecture-Diagrams.md)

This page answers one question:

What happens after someone calls an API route?

## Short Answer

The API receives input, calls a route-specific service path, and returns JSON. The banana calculation route uses the full pipeline. Batch and ripeness routes call the same native client through dedicated services.

## Route Inventory

- `GET /banana`: calculation route with pipeline + DB stage + native calculation.
- `POST /batches/create` and `GET /batches/{id}/status`: batch management route through `BatchService`.
- `POST /ripeness/predict`: ripeness prediction route through `RipenessService`.

## Full Walkthrough For GET /banana

1. Request reaches `BananaController`.
2. `BananaController` calls `BananaService.Calculate`.
3. `BananaService` creates `PipelineContext` and calls `PipelineExecutor<PipelineContext>`.
4. Pipeline steps execute in order:
   - `ValidationStep`
   - `DatabaseAccessStep`
   - `NativeCalculationStep`
   - `PostProcessingStep`
   - `AuditStep`
5. `DatabaseAccessStep` runs the configured DB client through `IDataAccessPipelineClient`.
6. `NativeCalculationStep` calls `INativeBananaClient`.
7. `BananaController` writes DB metadata headers:
   - `X-Banana-Db-Contract`
   - `X-Banana-Db-Source`
   - `X-Banana-Db-RowCount`
8. API returns `BananaResponse`.

```mermaid
flowchart TD
  A[GET /banana] --> B[BananaController]
  B --> C[BananaService]
  C --> D[PipelineExecutor]
  D --> E[ValidationStep]
  E --> F[DatabaseAccessStep]
  F --> G[NativeCalculationStep]
  G --> H[PostProcessingStep]
  H --> I[AuditStep]

  F --> J[IDataAccessPipelineClient]
  J --> K[NativeDalDbDataAccessClient]
  J --> L[ManagedNpgsqlDataAccessClient]

  G --> M[INativeBananaClient]
  M --> N[NativeBananaClient]
  N --> O[NativeMethods wrapper ABI]
  O --> P[src/native/core/domain]
  O --> Q[src/native/core/dal/domain]
```

## Batch And Ripeness Paths

`BatchController` and `RipenessController` do not execute `PipelineExecutor` today.

They call service classes directly:

- `BatchService` calls native batch lifecycle operations (`CreateBatch`, `GetBatchStatus`).
- `RipenessService` validates telemetry input and calls `PredictBatchRipeness`.
- Both go through `INativeBananaClient` and wrapper exports.

## Database Modes For GET /banana

- `NativeDal`: uses wrapper DB projection path.
- `ManagedNpgsql`: uses managed Npgsql query path.

Both modes share `IDataAccessPipelineClient`, so the rest of the pipeline stays unchanged.

## Important Rules

- Managed code does not call native internals directly.
- Wrapper owns native memory allocation and free behavior.
- Core domain and DAL domain are internal native modules.
- Keep `BANANA_NATIVE_PATH` explicit for local API runs.

## Where To Debug

- Wrong `/banana` response: start in controller, service, and pipeline steps.
- Batch or ripeness route issue: start in `BatchService` or `RipenessService`.
- Native loading issue: check `NativeLibraryResolver` and `BANANA_NATIVE_PATH`.
- DB issue: confirm `DbAccess:Mode`, `BANANA_PG_CONNECTION`, and connection-string settings.
- Native behavior mismatch: inspect wrapper exports first, then native core modules.

## Read Next

1. [Why-We-Use-A-Wrapper.md](Why-We-Use-A-Wrapper.md)
2. [How-The-Database-Step-Works.md](How-The-Database-Step-Works.md)
3. [First-Day-Checklist.md](First-Day-Checklist.md)
