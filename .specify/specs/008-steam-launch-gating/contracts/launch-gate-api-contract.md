# Contract: Launch Gate API

## Purpose

Define machine-readable verification and status contracts used by native launch flows and web remediation UX.

## Versioning

- Base namespace: `/v1/launch-gate`
- Backward-compatible fields may be added in minor revisions.
- Breaking reason-code or policy behavior changes require version bump.

## Endpoint: Verify Launch Attempt

- `POST /v1/launch-gate/verify`
- Request body:
  - `attemptId` (string, required)
  - `mode` (enum, required): `development`, `staging-uat`, `production-steam-package`
  - `steamAssertion` (object, required in staging/prod)
  - `clientBuildChannel` (string, required)
  - `overrideContext` (object, optional)
- Response body:
  - `decision` (enum): `allow`, `deny`
  - `reasonCode` (enum): `OK`, `UNLINKED_ACCOUNT`, `SUSPENDED_OR_RESTRICTED`, `STALE_SESSION`, `STEAM_UNAVAILABLE`, `API_UNAVAILABLE`, `OFFLINE_UNVERIFIABLE`, `UNKNOWN_MODE`
  - `effectiveMode` (enum)
  - `isRemediable` (boolean)
  - `remediation` (object, optional)
  - `retryPolicy` (object): `retryable`, `retryAfterMs`, `maxRecommendedAttempts`
  - `auditRef` (string)

## Endpoint: Query Latest Gate Status

- `GET /v1/launch-gate/status/{attemptId}`
- Response body:
  - `attemptId`
  - `decision`
  - `reasonCode`
  - `decidedAt`
  - `effectiveMode`
  - `freshnessExpiresAt`
  - `remediation` (optional)

## Endpoint: Record Retry Intent

- `POST /v1/launch-gate/retry-intent`
- Request body:
  - `attemptId` (string, required)
  - `priorReasonCode` (string, required)
  - `clientObservedAt` (timestamp, required)
- Response body:
  - `retryAccepted` (boolean)
  - `nextAttemptId` (string)
  - `requiresFreshVerification` (boolean)

## Production Safety Rules

1. `effectiveMode=production-steam-package` MUST deny if Steam assertion is missing/invalid/expired/indeterminate.
2. Production mode MUST deny when account link/standing verification is unavailable.
3. `overrideContext` MAY influence behavior only in non-production modes.
4. Any override attempt in production returns `deny` with `reasonCode=UNKNOWN_MODE` or a policy-specific deny reason and an audit reference.

## Error Semantics

- `400` invalid request shape or unsupported mode.
- `401/403` caller authorization invalid for status visibility.
- `409` stale attempt context or superseded retry token.
- `503` upstream verification dependency unavailable (decision remains deny in production semantics).
