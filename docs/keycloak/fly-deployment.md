# Keycloak Fly Deployment (Per Environment)

This document defines the phase-1 Keycloak deployment contract for Banana.

## Topology

One Fly app per environment:

- `banana-keycloak-dev`
- `banana-keycloak-staging`
- `banana-keycloak-prod`

Each app should use an environment-local Postgres database and environment-local secrets.

## Issuer/JWKS conventions

For environment app name `<app>`:

- Issuer: `https://<app>.fly.dev/realms/banana`
- JWKS: `https://<app>.fly.dev/realms/banana/protocol/openid-connect/certs`

## Provisioned assets

- Fly config: `src/typescript/keycloak/fly.toml`
- Image build: `docker/keycloak.Dockerfile`
- Realm bootstrap template: `src/typescript/keycloak/realm/banana-realm.template.json`
- Deploy helper: `scripts/deploy-keycloak-fly.sh`

## Required deployment env vars

Set these before running the deploy script:

- `BANANA_KEYCLOAK_ENV` (`dev`, `staging`, `prod`)
- `KEYCLOAK_ADMIN`
- `KEYCLOAK_ADMIN_PASSWORD`
- `BANANA_KEYCLOAK_DB_URL` (JDBC URL, for example `jdbc:postgresql://.../keycloak`)
- `BANANA_KEYCLOAK_DB_USERNAME`
- `BANANA_KEYCLOAK_DB_PASSWORD`

Optional:

- `FLY_APP_NAME` (defaults to `banana-keycloak-<env>`)
- `BANANA_KEYCLOAK_REGION` (defaults to `iad`)

## Deploy

From repository root:

```bash
bash scripts/deploy-keycloak-fly.sh
```

Validate issuer/JWKS reachability after deploy:

```bash
bash scripts/check-keycloak-issuer.sh banana-keycloak-dev
```

## Secret matrix

Use unique values per environment for all secrets.

| Environment | Fly app | DB URL secret source | Admin user secret | Admin password secret |
| --- | --- | --- | --- | --- |
| dev | banana-keycloak-dev | env-local store | env-local store | env-local store |
| staging | banana-keycloak-staging | env-local store | env-local store | env-local store |
| prod | banana-keycloak-prod | env-local store | env-local store | env-local store |

## Realm bootstrap notes

The tracked realm template is a safe baseline and should be reviewed before production import:

- `banana-react-spa` is configured as a public client for Authorization Code + PKCE.
- `banana-api` is configured as a confidential/service-account-capable API client placeholder.

If your environment requires stricter redirect or web origin lists, update the realm template and re-import.
