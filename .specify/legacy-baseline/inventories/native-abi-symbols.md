# Native ABI Symbol Inventory (v1 snapshot)

Source: `src/native/wrapper/banana_wrapper.h`
Caller-side: `src/c-sharp/asp.net/NativeInterop/NativeMethods.cs` (uses `LibraryImport`).
Library name: `banana_native` (Windows: `banana_native.dll`, Linux: `libbanana_native.so`).
Discovery contract: ASP.NET resolves library via `BANANA_NATIVE_PATH` env var.

## Status codes (`NativeStatusCode`)

| Code | Name | Meaning |
| --- | --- | --- |
| 0 | Ok | Success |
| 1 | InvalidArgument | Caller passed bad input |
| 2 | Overflow | Numeric overflow |
| 3 | InternalError | Unexpected native failure |
| 4 | DbError | DAL call failed |
| 5 | DbNotConfigured | `BANANA_PG_CONNECTION` missing (loud gating) |
| 6 | NotFound | Lookup miss |

## Exports (21)

All return `int` status code. String/buffer outputs use **callee-allocates,
caller-frees-via-`banana_free`** ownership.

### Calculation domain
- `banana_calculate_banana(int purchases, int multiplier, int* out_banana)`
- `banana_calculate_banana_with_breakdown(...)` — extended breakdown variant
- `banana_create_banana_message(int purchases, int multiplier, char** out_message)` — caller frees `out_message` via `banana_free`

### DAL / profile
- `banana_db_query_banana_profile(...)` — DAL-gated, returns `DbNotConfigured` when `BANANA_PG_CONNECTION` unset

### Classifier / ML
- `banana_predict_banana_regression_score(...)`
- `banana_classify_banana_binary(...)`
- `banana_classify_banana_transformer(...)`
- `banana_classify_not_banana_junk(...)`
- `banana_predict_banana_ripeness(...)`

### Batch domain (UTF-8 string marshalling)
- `banana_create_batch(...)`
- `banana_get_batch_status(...)`
- `banana_predict_batch_ripeness(...)`

### Harvest batch domain (UTF-8)
- `banana_create_harvest_batch(...)`
- `banana_add_bunch_to_harvest_batch(...)`
- `banana_get_harvest_batch_status(...)`

### Truck/logistics domain (UTF-8)
- `banana_register_truck(...)`
- `banana_load_truck_container(...)`
- `banana_unload_truck_container(...)`
- `banana_relocate_truck(...)`
- `banana_get_truck_status(...)`

### Memory
- `banana_free(void* pointer)` — frees any callee-allocated buffer returned via out-pointer

## v2 Constraints (see spec 006)

- Total exports must remain ≤ today's count (21) without explicit ABI rev.
- All new exports must follow the int-status + out-pointer + `banana_free`
  pattern. No managed-friendly returns from native.
- Callers MUST treat `DbNotConfigured` as configuration error, not runtime
  fault — the gate is intentional.
