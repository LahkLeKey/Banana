# API Service Context

## Purpose

Define backend service behavior, domain use cases, and service-side integration contracts.

## In scope

- Application/domain logic and route behavior
- Persistence and composition concerns
- Native integration calls from service code

## Out of scope

- Client rendering and UX behavior
- Native engine implementation internals

## Ubiquitous language

- Use case: Service-level operation that captures business intent
- Contract: Request/response or integration boundary guaranteed by the service
- Identity authority: External service whose signing keys and issuer identity are trusted for API bearer token verification
- Auth cutover mode: Explicit runtime mode that selects one authentication authority contract at a time
- Token authority check: Verification of bearer token signature, issuer, audience, and expiry against identity authority metadata

## Core invariants

- Public API contracts must be explicit and version-safe
- Domain rules should be enforceable independently from transport/framework concerns
- API authentication must trust one configured identity authority contract at a time
- Keycloak is the single authentication authority for API runtime behavior
- Phase-1 Keycloak auth accepts tokens via token authority checks without local session-store gating
- When Keycloak mode is active, protected-route authorization fails closed if issuer/JWKS/audience configuration is invalid

## Key seams

- Native C ABI integration boundary
- Client-facing HTTP/schema contracts
- Shared TypeScript packages consumed by API and client

## Open questions

- Steam authentication paths are retired and out of scope for active runtime auth behavior
- Keycloak deployment topology is one Fly app per environment to limit blast radius
- Phase-1 API auth mode is JWT verification via Keycloak issuer/JWKS only
- Keycloak rollout mode defaults to enabled and hard-fails startup on invalid configuration
