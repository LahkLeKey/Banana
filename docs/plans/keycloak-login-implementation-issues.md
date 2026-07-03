# Keycloak Login Migration: 4 Implementation Issues

- Source PRD: docs/plans/keycloak-login-prd.md
- Steam purge inventory: docs/plans/steam-auth-purge-inventory.md
- Execution model: one fresh session per issue using `/implement`

## Issue 1: Provision Keycloak on Fly (per-environment topology)

### Scope
- Provide reproducible local Docker Compose Keycloak + Postgres startup for auth iteration.
- Create Fly deployment assets for Keycloak as a dedicated app per environment.
- Define per-environment app naming, issuer URL conventions, and secret matrix.
- Provision or wire dedicated database per environment.

### Deliverables
- Local compose profile/scripts for Keycloak dashboard bring-up and readiness verification.
- Infrastructure config files and scripts for dev/staging/prod Keycloak apps.
- Environment variable contract document for Keycloak deployment.
- Initial realm/client bootstrap import path and instructions.

### Acceptance criteria
- Local Keycloak dashboard is reachable from integrated terminal/browser via documented commands.
- At least one environment is deployable end-to-end with documented steps.
- Issuer and JWKS endpoints are reachable from API runtime environment.
- Existing API Fly app config remains unchanged in this issue.

---

## Issue 2: Frontend `/login` + OIDC PKCE integration

### Scope
- Route `/login` to active login page.
- Implement Keycloak Authorization Code + PKCE redirect/callback flow.
- Keep bearer token in memory only.
- Add protected-route redirect behavior.

### Deliverables
- Router wiring updates and PKCE auth client module.
- Callback handling for success/failure/state validation paths.
- Config validation that blocks startup when required auth vars are invalid.

### Acceptance criteria
- `/login` reliably launches Keycloak auth flow.
- Callback completes into authenticated app state with in-memory token.
- Reload clears auth state and triggers reauth path as expected.

---

## Issue 3: API JWT authority checks with Keycloak defaults

### Scope
- Implement Keycloak JWKS-based bearer verification.
- Validate issuer, audience, signature, and expiry.
- Default runtime mode to Keycloak and fail startup on invalid auth config.
- Remove Steam auth implementations from active API runtime auth codepaths.

### Deliverables
- Auth middleware/service updates for Keycloak token authority checks.
- Startup-time config validation and explicit failure messaging.
- Integration tests for invalid issuer/audience/signature/expiry and missing token.

### Acceptance criteria
- Protected endpoints authorize only valid Keycloak-issued tokens.
- Misconfigured auth environment causes deterministic startup failure.
- Legacy Steam auth paths are removed from active API runtime flow.

---

## Issue 4: Rollout hardening, tests, and operations docs

### Scope
- Add end-to-end smoke checks for login and protected API access.
- Publish runbooks for deploy, secret rotation, and incident rollback.
- Validate cross-environment behavior under one-app-per-environment model.
- Remove Steam-auth references from active auth docs and rollout acceptance criteria.

### Deliverables
- Smoke scripts/checklist for non-local environment validation.
- Operations docs for credential rotation and auth outage response.
- Final migration checklist for enabling in each environment.

### Acceptance criteria
- Team can execute rollout checklist without undocumented steps.
- Smoke checks catch broken issuer/JWKS/login path before release.
- Rollback path is documented and tested for one environment.

## Session handoff template (use per issue)

1. Include PRD link and issue section reference.
2. Include locked decisions from ADRs and CONTEXT files.
3. Include explicit out-of-scope list for that issue.
4. Require test evidence in final implementation report.
