# Keycloak setup and frontend/API integration

This guide defines Banana's auth baseline:

- Keycloak realm configuration is the main authentication manager.
- Frontend and API consume the Keycloak realm contract.
- Third-party providers are configured in Keycloak, not in application code.

## Scope

This document covers:

- Keycloak Fly deployment for dev/staging/prod
- realm bootstrap and identity provider wiring
- frontend integration points
- API integration points
- production/Fly alignment

## Source of truth

Keycloak assets in this repository:

- Realm template: src/typescript/keycloak/realm/banana-realm.template.json
- Fly deployment guide: docs/keycloak/fly-deployment.md
- API Fly deployment guide: docs/keycloak/fly-api-deployment.md
- Identity providers guide: docs/keycloak/identity-providers.md
- Deploy helper: scripts/deploy-keycloak-fly.sh
- Issuer/JWKS readiness check: scripts/check-keycloak-issuer.sh

Realm-level provider and user-login behavior should be changed in the realm template and imported into Keycloak, then consumed by app code through the start/callback contract.

## Dev setup (Fly)

1. Set deployment env vars (see docs/keycloak/fly-deployment.md).
2. Deploy or update dev Keycloak:

```bash
export BANANA_KEYCLOAK_ENV=dev
bash scripts/deploy-keycloak-fly.sh
```

3. Validate dev issuer/JWKS endpoints:

```bash
bash scripts/check-keycloak-issuer.sh banana-keycloak-dev
```

Issuer convention:

- https://banana-keycloak-dev.fly.dev/realms/banana

Current decision:

- Keep dev auth on the Fly-managed `*.fly.dev` issuer.
- Do not move dev to a custom auth domain until an explicit cutover plan is approved.

Authority mapping contract:

- Production authority host is `kc-idp.banana.engineer` (production only).
- Canonical dev authority host is `banana-keycloak-dev.fly.dev`.
- Dev runtime and dev callback verification must not depend on production authority host.

Dev callback allowlist contract for `banana-react-spa`:

- `http://localhost:8081/auth/keycloak/callback`
- `http://127.0.0.1:8081/auth/keycloak/callback`
- `https://api.banana.engineer/auth/keycloak/callback`
- `https://banana-api.fly.dev/auth/keycloak/callback`

## Frontend integration contract

Frontend should initiate login by calling API start endpoint:

- GET /auth/keycloak/start

Optional query parameters:

- returnTo: where browser should end after callback handoff
- provider: provider alias for Keycloak hint (github, google, linkedin)

Do not implement provider-specific OAuth flows in frontend. Frontend should only choose provider alias and route through API start endpoint.

## API integration contract

API auth endpoints:

- GET /auth/keycloak/start
- GET /auth/keycloak/callback
- GET /auth/session
- POST /auth/logout

Behavior:

1. /auth/keycloak/start builds Keycloak authorization URL and forwards provider as kc_idp_hint.
2. /auth/keycloak/callback exchanges authorization code against Keycloak token endpoint.
3. API derives user subject from Keycloak-issued token payload.
4. API mints Banana app token for current phase session continuity.

API should not contain provider-specific OAuth endpoint logic for GitHub/Google/LinkedIn. That ownership belongs to Keycloak realm provider config.

## Realm operational model

- Realm owns identity providers, broker settings, login behavior, and user identity lifecycle.
- Apps consume issued tokens and subject claims.
- New services must integrate against Keycloak realm contract instead of introducing parallel auth authorities.

## Environment alignment

Use one Keycloak Fly app per environment and keep issuer/JWKS aligned with app name as documented in docs/keycloak/fly-deployment.md.

Before promoting environment changes:

1. Validate dev issuer/JWKS reachability.
2. Validate frontend start/callback flow against target environment.
3. Validate API protected routes using Keycloak-issued tokens.

## Dev verification runbook

Use these checks whenever auth routing or Keycloak realm allowlists change:

```bash
flyctl auth login
flyctl apps list
flyctl secrets list -a <dev-api-app>
bash scripts/check-keycloak-issuer.sh banana-keycloak-dev
bash scripts/check-auth-start-redirect-integrity.sh
```

Expected behavior:

- Auth-start for provider `github` routes through `banana-keycloak-dev.fly.dev`.
- No `Invalid parameter: redirect_uri` errors appear for approved callback hosts.
=======
>>>>>>> 31691ac7 (chore(auth): migrate dev keycloak to fly and guard against localhost issuer drift)

## Troubleshooting

- Missing provider credentials: login fails when selecting that provider.
- Wrong provider alias: Keycloak hint ignored or provider not found.
- Callback mismatch: verify provider redirect URI and Keycloak realm broker endpoint.
- Issuer/JWKS failures: run scripts/check-keycloak-issuer.sh for the target Fly app.
