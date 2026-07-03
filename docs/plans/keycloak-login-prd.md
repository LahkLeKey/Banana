# PRD: Keycloak-First Login Migration (Phase 1)

- Date: 2026-06-30
- Owner: Banana engineering
- Status: Ready for implementation slicing

## Problem statement

The current auth path is split across Steam-centric API routes and guest/local frontend behavior, while the frontend route wiring for `/login` is inconsistent with intended UX. This creates migration friction and operational ambiguity for production auth.

## Goals

- Make Keycloak the sole authentication authority in phase 1.
- Make local Docker Compose the primary auth iteration path before production deploys.
- Deploy Keycloak on Fly.io with one app per environment.
- Use OIDC Authorization Code + PKCE in the React client.
- Use API-side JWT verification against Keycloak issuer/JWKS/audience/expiry.
- Remove Steam auth implementations and Steam-auth documentation from active product auth flows.
- Default to Keycloak mode and hard-fail startup when configuration is invalid.

## Non-goals (phase 1)

- Session-store-gated authorization checks.
- Advanced token revocation beyond JWT expiry and issuer controls.

## Decisions locked

- Identity authority: Keycloak-only for active auth flows.
- Deployment topology: one Fly Keycloak app per environment.
- Local iteration gate: Docker Compose Keycloak + Postgres must pass local dashboard/login/API checks before Fly rollout.
- API auth mode: pure JWT authority checks (issuer/JWKS/audience/expiry).
- Frontend flow: OIDC Authorization Code + PKCE.
- Frontend token storage: in-memory only.
- Rollout mode: default enabled; hard-fail startup on invalid auth config.

## User-facing outcomes

- Visiting `/login` opens Keycloak-backed sign-in flow.
- Successful login returns user to app with valid bearer token in memory.
- Protected routes and API calls enforce Keycloak-issued bearer token validity.
- Invalid auth config blocks startup instead of silently degrading security.

## Functional requirements

1. Frontend route and auth UX
- `/login` must render the active Keycloak login flow.
- Callback handling must validate state/PKCE and handle error paths.
- Protected-route guard must redirect unauthenticated users to login.

2. API token verification
- Bearer verification must validate signature, issuer, audience, expiry.
- API must reject invalid or missing token with explicit 401 responses.

3. Keycloak deployment and environment config
- Dedicated Fly app and DB per environment.
- Environment-specific issuer URLs and client configuration.
- Startup config validation must fail fast when required vars are missing/invalid.

4. Operability
- Clear runbook for secret setup and credential rotation.
- Smoke checks for issuer/JWKS reachability and login round-trip.

## Constraints

- Monorepo architecture and existing Fly API deployment must remain stable.
- Existing API `fly.toml` should remain untouched during initial Keycloak infra bring-up.
- Phase-1 scope prioritizes low blocker risk over full legacy compatibility.

## Risks and mitigations

- Risk: Auth outage from misconfiguration.
  - Mitigation: fail-fast startup + environment-specific runbooks + smoke checks.
- Risk: Client token lifecycle bugs.
  - Mitigation: callback/expiry/error-path tests and explicit auth-state machine.
- Risk: Incomplete route migration.
  - Mitigation: guarded acceptance criteria for `/login` mapping and protected routes.

## Acceptance criteria

- Local Docker Compose stack starts Keycloak dashboard successfully and is reproducible from integrated terminal commands.
- Local login callback and protected API call pass before non-local rollout checks begin.
- Keycloak apps are running in target Fly environments with isolated config/secrets.
- React `/login` flow authenticates via PKCE and stores token in memory only.
- API rejects invalid issuer/audience/signature/expiry tokens.
- Startup hard-fails on invalid or missing required auth configuration.
- Smoke tests and runbooks exist and are verified in at least one non-local environment.
