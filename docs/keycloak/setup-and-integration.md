# Keycloak setup and frontend/API integration

This guide defines Banana's auth baseline:

- Keycloak realm configuration is the main authentication manager.
- Frontend and API are consumers of the Keycloak realm contract.
- Third-party providers are configured in Keycloak, not in application code.

## Scope

This document covers:

- local Keycloak startup
- realm bootstrap and identity provider wiring
- frontend integration points
- API integration points
- production/Fly alignment

## Source of truth

Keycloak assets in this repository:

- Realm template: src/typescript/keycloak/realm/banana-realm.template.json
- Local startup helper: scripts/keycloak-local.sh
- Local readiness check: scripts/check-keycloak-local.sh
- Fly deployment guide: docs/keycloak/fly-deployment.md
- Identity providers guide: docs/keycloak/identity-providers.md

Realm-level provider and user-login behavior should be changed in the realm template and imported into Keycloak, then consumed by app code through the start/callback contract.

## Local setup

1. Start Keycloak and local Keycloak Postgres:

```bash
bash scripts/keycloak-local.sh up
```

2. Validate issuer/JWKS endpoints:

```bash
bash scripts/keycloak-local.sh check
```

3. Open admin console:

- http://localhost:8080/admin

## GitHub IdP manual setup walkthrough

If you want a guided manual flow, use:

```bash
bash scripts/setup-github-idp-manual.sh
```

This helper script will:

- create `.env.keycloak.local` from template when missing
- print all manual setup links and required callback values
- optionally write GitHub client credentials into `.env.keycloak.local`

Direct links:

- GitHub OAuth Apps: https://github.com/settings/developers
- Keycloak Identity Providers (banana realm): http://localhost:8080/admin/master/console/#/banana/identity-providers

GitHub OAuth App values:

- Homepage URL: `http://localhost:5173`
- Authorization callback URL: `http://localhost:8080/realms/banana/broker/github/endpoint`

If you already have credentials, you can write and bootstrap in one command:

```bash
bash scripts/setup-github-idp-manual.sh \
	--client-id "<github-client-id>" \
	--client-secret "<github-client-secret>" \
	--bootstrap
```

## Reproducible realm automation (dev container)

Use the bootstrap script instead of manual console setup.

1. Create local env file from template:

```bash
cp docs/keycloak/env.keycloak.local.example .env.keycloak.local
```

2. Fill GitHub provider credentials in `.env.keycloak.local`.

3. Run bootstrap (idempotent reconcile):

```bash
bash scripts/keycloak-realm-bootstrap.sh bootstrap
```

For a full local reset and re-import:

```bash
bash scripts/keycloak-realm-bootstrap.sh reset-bootstrap
```

Status check:

```bash
bash scripts/keycloak-realm-bootstrap.sh status
```

What the script reconciles:

- starts local Keycloak profile if needed
- applies `banana-react-spa` redirect/web-origin contract
- clears local PKCE enforcement for current API callback flow
- upserts GitHub identity provider when credentials are present

## Identity providers

Set provider credentials before startup or restart:

```bash
export BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID="..."
export BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET="..."

export BANANA_KEYCLOAK_IDP_GOOGLE_CLIENT_ID="..."
export BANANA_KEYCLOAK_IDP_GOOGLE_CLIENT_SECRET="..."

export BANANA_KEYCLOAK_IDP_LINKEDIN_CLIENT_ID="..."
export BANANA_KEYCLOAK_IDP_LINKEDIN_CLIENT_SECRET="..."
```

Then restart local Keycloak import:

```bash
bash scripts/keycloak-local.sh down
bash scripts/keycloak-local.sh up
```

Provider aliases expected by frontend/API hinting:

- github
- google
- linkedin

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

## Production/Fly alignment

Use one Keycloak Fly app per environment and keep issuer/JWKS aligned with app name as documented in docs/keycloak/fly-deployment.md.

Before promoting environment changes:

1. Validate local realm/provider behavior.
2. Validate dev issuer/JWKS reachability.
3. Validate frontend start/callback flow against target environment.

## Troubleshooting

- Missing provider credentials: login fails when selecting that provider.
- Wrong provider alias: Keycloak hint ignored or provider not found.
- Callback mismatch: verify provider redirect URI and Keycloak realm broker endpoint.
- Issuer/JWKS failures: run scripts/check-keycloak-local.sh locally or scripts/check-keycloak-issuer.sh for Fly environment.
