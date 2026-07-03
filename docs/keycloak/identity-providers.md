# Keycloak Identity Providers (GitHub, Google, LinkedIn)

This repo now imports three third-party identity providers into the `banana` realm:
- GitHub (`github`)
- Google (`google`)
- LinkedIn (`linkedin` via OIDC)

Keycloak realm configuration is the canonical identity manager. Frontend and API layers should route through Keycloak rather than implementing provider-specific OAuth logic directly.

## Frontend and API integration model

End-user login path:

1. Frontend calls API route `GET /auth/keycloak/start`.
2. Frontend may include `provider=<alias>` where alias is one of `github`, `google`, `linkedin`.
3. API redirects to Keycloak authorization endpoint and forwards provider hint as `kc_idp_hint`.
4. Keycloak handles upstream provider OAuth/OIDC flow and returns to API callback.
5. API callback exchanges authorization code with Keycloak token endpoint, resolves user subject, then redirects to frontend with app token hash payload.

This keeps all provider contracts in one place (Keycloak realm) while frontend/API remain thin consumers.

## Local setup

Set these environment variables before starting local Keycloak:

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

## OAuth redirect URI to register

For each provider app, include this callback URI:

- `http://localhost:8080/realms/banana/broker/<provider-alias>/endpoint`

Provider aliases in this realm:
- GitHub: `github`
- Google: `google`
- LinkedIn: `linkedin`

Examples:
- `http://localhost:8080/realms/banana/broker/github/endpoint`
- `http://localhost:8080/realms/banana/broker/google/endpoint`
- `http://localhost:8080/realms/banana/broker/linkedin/endpoint`

## Production Fly setup (GitHub)

Use the Fly Keycloak app callback URI in your GitHub OAuth App for production:

- `https://banana-keycloak-prod.fly.dev/realms/banana/broker/github/endpoint`

Deploy and sync Keycloak + GitHub IdP in one flow:

```bash
export BANANA_KEYCLOAK_ENV=prod
export KEYCLOAK_ADMIN="<prod-admin-user>"
export KEYCLOAK_ADMIN_PASSWORD="<prod-admin-password>"
export BANANA_KEYCLOAK_DB_URL="jdbc:postgresql://<prod-host>:5432/<keycloak-db>"
export BANANA_KEYCLOAK_DB_USERNAME="<prod-db-user>"
export BANANA_KEYCLOAK_DB_PASSWORD="<prod-db-password>"

export BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID="<github-client-id>"
export BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET="<github-client-secret>"

bash scripts/deploy-keycloak-fly.sh
```

If Keycloak is already deployed, you can sync only the GitHub IdP configuration:

```bash
export BANANA_KEYCLOAK_ENV=prod
export KEYCLOAK_ADMIN="<prod-admin-user>"
export KEYCLOAK_ADMIN_PASSWORD="<prod-admin-password>"
export BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID="<github-client-id>"
export BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET="<github-client-secret>"

bash scripts/keycloak-fly-github-idp.sh
```

## LinkedIn endpoint overrides (optional)

Defaults are already configured for LinkedIn OIDC. Override only if needed:

```bash
export BANANA_KEYCLOAK_IDP_LINKEDIN_AUTH_URL="https://www.linkedin.com/oauth/v2/authorization"
export BANANA_KEYCLOAK_IDP_LINKEDIN_TOKEN_URL="https://www.linkedin.com/oauth/v2/accessToken"
export BANANA_KEYCLOAK_IDP_LINKEDIN_USERINFO_URL="https://api.linkedin.com/v2/userinfo"
export BANANA_KEYCLOAK_IDP_LINKEDIN_JWKS_URL="https://www.linkedin.com/oauth/openid/jwks"
export BANANA_KEYCLOAK_IDP_LINKEDIN_ISSUER="https://www.linkedin.com"
```

## Notes

- For now this enables website sign-in identity memory via third-party brokers while app-side authorization remains minimal.
- If credentials are empty, provider login will fail at runtime for that provider; fill all required client values before testing sign-in.
- Keep provider alias names stable (`github`, `google`, `linkedin`) to avoid frontend/API hint drift.
