# Data Model: Domain Value Delivery Follow-Up (Phase 2)

**Branch**: `005-domain-value-delivery-followup` | **Phase**: 1 | **Date**: 2026-04-26

## Entities

### `NativeRuntimeError`

**Purpose**: Dedicated API payload for native runtime loading failures.

| Field | Type | Constraint |
|-------|------|------------|
| `error` | `string` | Must be `native_unavailable` |
| `remediation` | `string` | Must reference `BANANA_NATIVE_PATH` |
| `details` | `string?` | Optional diagnostic string (non-sensitive) |

### `BinaryClassificationResult`

**Purpose**: Typed mapped response for `/ml/binary`.

| Field | Type | Constraint |
|-------|------|------------|
| `label` | `string` | Non-empty |
| `confidence` | `double` | `0.0 <= value <= 1.0` |
| `model` | `string` | Non-empty |

### `NotBananaClassificationResult`

**Purpose**: Typed mapped response for `/not-banana/junk`.

| Field | Type | Constraint |
|-------|------|------------|
| `label` | `string` | Non-empty |
| `confidence` | `double` | `0.0 <= value <= 1.0` |
| `model` | `string` | Non-empty |

### `RipenessRouteRequest` (TS API)

**Purpose**: Fastify schema for ripeness route request body.

| Field | Type | Constraint |
|-------|------|------------|
| `inputJson` | `string` | Required, non-empty after trim |

### `RipenessRouteResponse` (TS API)

**Purpose**: Fastify schema for successful ripeness response.

| Field | Type | Constraint |
|-------|------|------------|
| `label` | `string` | One of `ripe`, `unripe`, `overripe` |
| `confidence` | `number` | `0.0 <= value <= 1.0` |
| `model` | `string` | Non-empty |

### `HarvestBatchResult`

**Purpose**: Typed mapped payload for harvest operations.

| Field | Type | Constraint |
|-------|------|------------|
| `batchId` | `string` | Non-empty |
| `status` | `string` | Non-empty |
| `metrics` | `object?` | Optional operation-specific metadata |

### `TruckResult`

**Purpose**: Typed mapped payload for truck operations.

| Field | Type | Constraint |
|-------|------|------------|
| `truckId` | `string` | Non-empty |
| `status` | `string` | Non-empty |
| `location` | `string?` | Optional, non-empty when present |
| `containers` | `array?` | Optional operation-specific list |

## Relationships

- Native interop returns JSON string payloads.
- Mapping services convert JSON string payloads to typed result models.
- Controllers return typed result models and rely on `StatusMapping` for errors.
- Fastify ripeness route validates `RipenessRouteRequest`, proxies to ASP.NET, and returns `RipenessRouteResponse`.
- e2e contract tests validate both success and failure shapes for the above entities.

## State/Status Notes

- `NativeRuntimeError` is emitted only when interop runtime load/symbol issues occur.
- Harvest/truck operations map `NativeStatusCode.NotFound` to HTTP 404.
- Validation errors map to HTTP 400 with deterministic payload shape across ASP.NET and TS API layers.