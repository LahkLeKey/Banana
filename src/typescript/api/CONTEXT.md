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
- Account profile: Banana-owned application profile record for user-facing account data and preferences
- Identity credential provider: Login method managed by identity authority (for example GitHub or email/password)
- Profile ownership split: Division where identity proof remains in Keycloak while app profile data is stored in Banana persistence
- Provider profile fallback: Identity-provider-sourced profile field used when no Banana override exists
- Self-service account lifecycle: User-managed sign-up, sign-in, and password reset flow handled by identity authority
- Guest access retirement: Decision that anonymous guest sessions are no longer part of the active account model
- Account rollout epic: Multi-story delivery track that introduces local account credentials and Banana-owned profiles without forcing all auth/profile changes into one slice
- Thin auth entry story: Smallest deliverable slice that adds a new credential path while deferring profile editing and guest migration concerns
- Environment authority mapping: Explicit rule that each runtime environment must use its own identity authority host and not cross-wire to another environment
- Callback host allowlist: Enumerated API callback hosts that the identity authority client must accept for one environment
- Hybrid dev runtime topology: Local API execution with Fly-hosted dev identity authority and database for persistent shared dev data
- Redirect integrity smoke check: Automated verification that auth-start redirects resolve to expected authority and callback hosts without identity-authority error pages

## Core invariants

- Public API contracts must be explicit and version-safe
- Domain rules should be enforceable independently from transport/framework concerns
- API authentication must trust one configured identity authority contract at a time
- Keycloak is the single authentication authority for API runtime behavior
- Keycloak-mediated GitHub and email/password are supported sign-in providers for phase-1 account access
- Identity provider linking is manual-only in phase 1 and must not auto-link by email match
- Phase-1 account profile ownership is hybrid: Keycloak remains identity authority, Banana persists app-owned profile fields
- Phase-1 account profiles support editable display name, bio, locale, avatar override, and notification preferences
- Phase-1 avatar resolution prefers Banana avatar override and falls back to provider profile avatar
- Phase-1 generic account access includes self-service sign-up and password reset through Keycloak-managed credentials
- Guest authentication is removed from the active API auth model in the generic account profile rollout
- Phase-1 Keycloak auth accepts tokens via token authority checks without local session-store gating
- The first delivery slice under the account rollout epic adds email/password sign-in and self-service sign-up beside GitHub
- The first delivery slice defers password reset, advanced profile editing, provider linking UI, and guest-auth removal
- When Keycloak mode is active, protected-route authorization fails closed if issuer/JWKS/audience configuration is invalid
- Dev auth must use dev identity authority configuration and must not rely on production authority hosts
- Dev Keycloak client redirect allowlist must include every supported dev API callback host used by login start flows
- Dev local API mode may authenticate against Fly-hosted dev Keycloak and dev database while preserving environment authority mapping
- Dev callback allowlist includes local (`localhost`/`127.0.0.1`) and approved hosted API callback hosts when those hosts are used for login start
- Auth rollout stories are blocked when environment authority mapping or redirect allowlist integrity is unresolved
- CI must run an auth-start redirect smoke check for provider=github against approved dev hosts via `.github/workflows/banana.yml` and `scripts/check-auth-start-redirect-integrity.sh`

## Key seams

- Native C ABI integration boundary
- Client-facing HTTP/schema contracts
- Shared TypeScript packages consumed by API and client

## Open questions

- Steam authentication paths are retired and out of scope for active runtime auth behavior
- Keycloak deployment topology is one Fly app per environment to limit blast radius
- Phase-1 API auth mode is JWT verification via Keycloak issuer/JWKS only
- Keycloak rollout mode defaults to enabled and hard-fails startup on invalid configuration
