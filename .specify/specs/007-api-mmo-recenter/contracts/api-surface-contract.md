# Contract: API Surface (DX12 + Website)

## Versioning Policy

- Base path: `/v1`
- Backward-compatible additions allowed in minor revisions.
- Breaking changes require a new major namespace (`/v2`) and migration notice.

## DX12 Gameplay-Oriented Endpoints (`/v1/gameplay/*`)

## Session Admission

- `POST /v1/gameplay/sessions/admissions`
- Request fields:
  - `playerId` (string, required)
  - `queueIntent` (string enum, required)
  - `idempotencyKey` (string, required)
- Response:
  - `sessionId`
  - `admissionStatus`
  - `continuityWindowExpiresAt`

## In-Session State Command

- `POST /v1/gameplay/sessions/{sessionId}/commands`
- Request fields:
  - `playerId` (string, required)
  - `commandType` (string, required)
  - `commandPayload` (object, required)
  - `commandSequence` (integer, required)
  - `idempotencyKey` (string, required)
- Response:
  - `authoritativeTick`
  - `resolutionCode`
  - `conflictHandled` (boolean)

## Session Rejoin

- `POST /v1/gameplay/sessions/{sessionId}/rejoin`
- Request fields:
  - `playerId` (string, required)
  - `continuityToken` (string, required)
- Response:
  - `rejoinStatus`
  - `restoredSnapshotRef`

## Session Termination

- `POST /v1/gameplay/sessions/{sessionId}/terminate`
- Request fields:
  - `playerId` (string, required)
  - `terminationReason` (string, required)
  - `idempotencyKey` (string, required)
- Response:
  - `terminationStatus`
  - `outcomeRef`

## Website Player Endpoints (`/v1/player/*`)

## Account View

- `GET /v1/player/account`
- Response:
  - `playerId`
  - `accountStatus`
  - `profile`
  - `version`

## Account Update

- `PATCH /v1/player/account`
- Request fields:
  - `profilePatch` (object, required)
  - `expectedVersion` (integer, required)
  - `idempotencyKey` (string, required)
- Response:
  - `accountStatus`
  - `version`

## Progression + Inventory View

- `GET /v1/player/progression`
- `GET /v1/player/inventory`
- Response includes freshness metadata and authoritative snapshot references.

## Insights View

- `GET /v1/player/insights`
- Response:
  - `sessionSummary`
  - `progressionSummary`
  - `inventoryTrendSummary`
  - `noData`
  - `freshnessTimestamp`

## Error + Authorization Semantics

- Validation failure: `400` with domain error code.
- Authorization failure: `401` or `403` with actor-scope reason.
- Conflict/version mismatch: `409` with deterministic resolution hint.
- Transient persistence/runtime failure: `503` with retry guidance.

## Compatibility Commitments

1. Existing auth/session route behavior remains backward-compatible while new `/v1` namespaces are introduced.
2. New fields are additive and optional by default unless required by explicitly versioned contract.
3. Deprecations require published sunset notice in release notes and contract docs.
