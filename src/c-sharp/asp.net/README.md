# Banana ASP.NET Runtime Notes

## Native Runtime Contract

If native interop cannot load the Banana library or required symbols, endpoints that depend on native calls return HTTP 503 with:

- `error`: `native_unavailable`
- `remediation`: `Set BANANA_NATIVE_PATH to a valid native library path.`

This contract distinguishes native runtime availability issues from database configuration (`BANANA_PG_CONNECTION`) concerns.

## Operational Endpoint Surface

Harvest endpoints:

- `POST /harvest/create`
- `POST /harvest/{batchId}/bunches`
- `GET /harvest/{batchId}/status`

Truck endpoints:

- `POST /trucks/register`
- `POST /trucks/{truckId}/containers/load`
- `POST /trucks/{truckId}/containers/{containerId}/unload`
- `POST /trucks/{truckId}/relocate`
- `GET /trucks/{truckId}/status`

All operational endpoints use the shared `StatusMapping` contract, including `native_unavailable` 503 responses when native interop is not available.
