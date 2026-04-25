# HTTP Route Inventory (v1 snapshot)

Source: `src/c-sharp/asp.net/Controllers/*.cs`. ASP.NET 8 (Kestrel).

## Controllers + routes

| Controller | Class route | Verb | Path | Calls native |
| --- | --- | --- | --- | --- |
| `BananaController` | `[controller]` -> `/banana` | GET | `/banana` | `banana_calculate_banana*`, `banana_create_banana_message`, `banana_db_query_banana_profile` |
| `BatchController` | `batches` | POST | `/batches/create` | `banana_create_batch`, `banana_create_harvest_batch`, `banana_add_bunch_to_harvest_batch`, `banana_register_truck`, `banana_load_truck_container`, `banana_unload_truck_container`, `banana_relocate_truck` |
| `BatchController` | `batches` | GET | `/batches/{id}/status` | `banana_get_batch_status`, `banana_get_harvest_batch_status`, `banana_get_truck_status` |
| `RipenessController` | `ripeness` | POST | `/ripeness/predict` | `banana_predict_banana_ripeness`, `banana_predict_batch_ripeness` |
| `NotBananaController` | `not-banana` | POST | `/not-banana/junk` | `banana_classify_not_banana_junk` |

## Persistence

Each controller persists request/response JSON to a per-domain Prisma table
via the Fastify TypeScript API peer (see `prisma-schema-snapshot.md`). The
ASP.NET API does **not** own writes; it forwards the call envelope and the
TypeScript API persists asynchronously. Per-route ownership table lives in
`.specify/specs/008-typescript-api/contracts/route-ownership.md`.

## v2 Constraints (specs 007, 008)

- Single typed `PipelineContext` per request; no DTO sprawl.
- Single interop seam in `NativeInterop/`; ≤2 fakes touched per change.
- Route shapes here are the **v1 baseline only**; v2 may rename, but new
  routes MUST appear in `008/contracts/route-ownership.md` first.
