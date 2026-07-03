# ADR-0001: Keycloak JWT Authority in Phase 1

- Status: accepted
- Date: 2026-06-30
- Context: API service

## Problem

The API currently mixes session persistence concerns with Steam-centric authentication flows. The Keycloak migration needs a low-friction first phase that avoids dual authority checks and reduces rollout blockers.

## Decision

In phase 1, the API accepts bearer tokens using Keycloak token authority checks only:

- signature verification via Keycloak JWKS
- issuer validation
- audience validation
- expiry validation

Keycloak mode is the default runtime mode for phase 1 and startup hard-fails when required Keycloak issuer/JWKS/audience configuration is invalid or missing.

Local Banana session-store checks are deferred for phase 1 and are not required to authorize protected API requests.

## Consequences

- Positive:
  - Simpler implementation and lower migration complexity
  - One clear authority contract for authentication in phase 1
  - Fewer stateful failure modes during rollout
  - Prevents accidental fail-open behavior caused by partial configuration
- Negative:
  - No server-managed session revocation gate in phase 1
  - Future token revocation controls require additional design
  - Misconfiguration causes immediate startup failure until corrected
- Follow-up actions:
  - Define revocation/forced-signout strategy for a future phase
  - Add integration coverage for invalid issuer/audience/signature/expiry paths

## Alternatives considered

- JWT verification plus local session-store gating in phase 1: rejected as unnecessary complexity during initial cutover.
- Keep Steam/session model as primary and defer Keycloak API trust: rejected to avoid parallel authority drift.
