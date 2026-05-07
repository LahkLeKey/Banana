# Data Model: API Parity Governance

## Entity: OverlappingRouteRecord

- Description: Canonical identity record for a capability exposed by both APIs.
- Fields:
  - route_key: deterministic key (`METHOD path`)
  - method: HTTP method (`GET|POST|PUT|PATCH|DELETE`)
  - path: canonical route path
  - aspnet_surface: source pointer (controller/action metadata)
  - fastify_surface: source pointer (domain/route registration metadata)
  - capability_area: domain tag (e.g., `chat`, `ripeness`, `not-banana`)
- Validation Rules:
  - `route_key` must be unique in inventory.
  - `aspnet_surface` and `fastify_surface` are both required for overlap records.

## Entity: ParityExpectation

- Description: Expected behavior contract attached to one overlap route.
- Fields:
  - route_key: foreign key to OverlappingRouteRecord
  - expected_statuses: set of allowed status outcomes by scenario
  - required_response_fields: required field set for successful response payloads
  - optional_response_fields: optional response field set
  - shape_version: semantic contract version for response field expectations
- Validation Rules:
  - At least one expected status is required.
  - Required fields must be deterministic and stable across both API surfaces.

## Entity: ParityDriftFinding

- Description: A single detected mismatch between observed parity and expected parity.
- Fields:
  - finding_id: unique identifier
  - route_key: affected overlap route
  - mismatch_type: `missing_route | status_mismatch | shape_mismatch`
  - severity: `blocker | high | medium`
  - aspnet_observed: observed ASP.NET behavior snapshot
  - fastify_observed: observed Fastify behavior snapshot
  - detected_at: timestamp
  - gate_blocking: boolean
- Validation Rules:
  - `gate_blocking` must be true when mismatch_type is unresolved and no active exception applies.
  - Observed payload must include enough detail for actionable remediation.

## Entity: ParityException

- Description: Approved temporary waiver for a specific parity drift scope.
- Fields:
  - exception_id: unique identifier
  - route_key: affected route key
  - mismatch_type: exact mismatch category being waived
  - owner: accountable approver/maintainer
  - rationale: business/technical justification
  - approved_at: approval timestamp
  - expires_at: mandatory expiration timestamp
  - status: `active | expired | revoked`
- Validation Rules:
  - Exception is valid only while `status = active` and current time < `expires_at`.
  - Exception scope must not cover additional mismatch types or additional routes.

## Relationships

- OverlappingRouteRecord (1) -> (1..n) ParityExpectation
- OverlappingRouteRecord (1) -> (0..n) ParityDriftFinding
- ParityDriftFinding (0..1) -> (1) ParityException when temporary waiver exists

## State Transitions

### ParityDriftFinding

- `detected -> triaged -> resolved`
- `detected -> triaged -> excepted` (only when active ParityException exists)
- `excepted -> expired` when exception expiration passes without route parity restoration

### ParityException

- `active -> expired` at `expires_at`
- `active -> revoked` by governance owner
- `expired|revoked` findings must return to blocking state unless parity is resolved
