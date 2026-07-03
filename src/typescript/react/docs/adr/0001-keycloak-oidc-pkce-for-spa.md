# ADR-0001: Keycloak OIDC PKCE for React SPA

- Status: accepted
- Date: 2026-06-30
- Context: client-applications

## Problem

The React client needs a secure Keycloak login flow that avoids secret material in the browser and remains compatible with phase-1 API bearer token verification.

## Decision

Use OIDC Authorization Code flow with PKCE for the React SPA as a public client.

- No client secret is stored in frontend runtime configuration
- Redirect-based login and callback complete token acquisition
- Client sends bearer tokens to API endpoints that validate issuer, audience, signature, and expiry
- Bearer tokens are stored in memory only during phase 1 (no localStorage/sessionStorage persistence)
- Keycloak mode is enabled by default and client startup hard-fails when required auth configuration is invalid

## Consequences

- Positive:
  - Aligns with modern SPA security guidance
  - Avoids browser-side client secret handling
  - Works cleanly with Keycloak issuer/JWKS validation in API phase 1
  - Reduces token persistence risk from browser storage compromise
  - Avoids partially configured, silently degraded protected-route behavior
- Negative:
  - Requires callback/state/nonce and token lifecycle handling in client code
  - Adds frontend complexity for redirect and refresh behavior
  - Full-page reload requires reauthentication or silent renewal strategy
  - Broken auth env configuration blocks app startup until repaired
- Follow-up actions:
  - Define exact callback route and failure UX behavior
  - Add test coverage for callback success, invalid state, and token expiry handling

## Alternatives considered

- Implicit flow: rejected due to weaker security properties and legacy status
- BFF-only login in phase 1: rejected to keep initial migration scope smaller
