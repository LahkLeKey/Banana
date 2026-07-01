# Client Applications Context

## Purpose

Define client-facing behavior, interaction flows, and integration with backend/runtime surfaces.

## In scope

- UI behavior and interaction patterns
- Client-side state and shell/runtime integration
- Consumption of API and shared package contracts

## Out of scope

- Server-side domain/persistence internals
- Native engine algorithm internals

## Ubiquitous language

- Flow: User-visible sequence of interactions and state transitions
- Client contract: Assumption the UI makes about API/shared package behavior
- Identity handoff flow: Redirect-and-return authentication sequence between client and identity authority
- Auth authority mode: Client runtime mode that determines which identity authority login flow is active
- Bearer continuity: Client behavior that carries the active bearer token across protected API calls until expiry or sign-out
- OIDC PKCE flow: Authorization Code flow with Proof Key for Code Exchange used by a public browser client without a client secret
- Ephemeral token storage: In-memory bearer token handling that does not persist tokens to localStorage or sessionStorage

## Core invariants

- User-visible behavior should be consistent across supported runtime modes
- Client assumptions about API contracts must remain explicit and testable
- Login flow must map to the active identity authority mode and avoid parallel authority assumptions
- Keycloak is the single authentication authority for active client login and bearer continuity
- Phase-1 protected API access relies on bearer continuity plus API-side token authority checks
- Keycloak browser login uses OIDC Authorization Code + PKCE and does not require a frontend client secret
- Phase-1 bearer tokens remain in ephemeral memory and are cleared on reload or sign-out
- In Keycloak mode, client startup blocks and surfaces configuration errors rather than silently degrading protected flows

## Key seams

- API service request/response boundaries
- Shared UI and resilience package boundaries

## Open questions

- Steam login paths are retired and out of scope for active client authentication
- Keycloak deployment topology is one Fly app per environment to isolate login incidents
- Phase-1 OIDC mode is Authorization Code + PKCE for the React client
- Phase-1 token storage mode is in-memory only
- Keycloak rollout mode defaults to enabled and hard-fails startup on invalid configuration
