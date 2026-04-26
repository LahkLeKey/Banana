# API Contracts: Domain Value Delivery Follow-Up (Phase 2)

**Branch**: `005-domain-value-delivery-followup` | **Phase**: 1 | **Date**: 2026-04-26

## ASP.NET Contracts

### Native unavailable error mapping

Any endpoint relying on native interop must return:

- HTTP `503`
- Body:

```json
{
  "error": "native_unavailable",
  "remediation": "Set BANANA_NATIVE_PATH to a valid native library path."
}
```

### ML contract ownership

`POST /ml/binary` and `POST /ml/transformer` return typed payloads via service/pipeline mapping. Controllers must not deserialize to raw `JsonElement` directly.

### Not-banana contract ownership

`POST /not-banana/junk` returns typed payload via service/pipeline mapping. Controller owns only route + status handling.

### Harvest HTTP endpoints

- `POST /harvest/create`
- `POST /harvest/{batchId}/bunches`
- `GET /harvest/{batchId}/status`

All endpoints return typed payloads and map:
- `NativeStatusCode.NotFound` -> HTTP `404`
- `NativeStatusCode.InvalidArgument` -> HTTP `400`
- `NativeUnavailable` -> HTTP `503`
- malformed native JSON payloads -> HTTP `500` with `{ "error": "invalid_native_payload" }`

### Truck HTTP endpoints

- `POST /trucks/register`
- `POST /trucks/{truckId}/containers/load`
- `POST /trucks/{truckId}/containers/{containerId}/unload`
- `POST /trucks/{truckId}/relocate`
- `GET /trucks/{truckId}/status`

All endpoints return typed payloads and apply the same status mapping policy as harvest endpoints.
- malformed native JSON payloads -> HTTP `500` with `{ "error": "invalid_native_payload" }`

## TypeScript API Contracts

### `POST /ripeness/predict` (Fastify)

**Request**:

```json
{
  "inputJson": "banana sample text"
}
```

**Response 200**:

```json
{
  "label": "ripe",
  "confidence": 0.81,
  "model": "regression"
}
```

**Response 400** (validation):

```json
{
  "error": "invalid_argument"
}
```

**Response 503** (upstream native unavailable):

```json
{
  "error": "native_unavailable",
  "remediation": "Set BANANA_NATIVE_PATH to a valid native library path."
}
```

## Testing Contract

- ASP.NET unit tests assert typed result payload shapes and status mapping.
- TS API route tests assert success + validation failure + upstream failure pass-through behavior.
- e2e contract lane asserts reproducible command and endpoint contracts for ripeness, harvest, and truck surfaces via `tests/e2e/Contracts/ApiContractLaneTests.cs`.