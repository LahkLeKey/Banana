# API Fly Deployment (Production-First)

This guide defines the Banana API deployment contract for Fly production.

## Scope

- API app deployment to Fly
- Production Keycloak issuer alignment
- Production database secret alignment

## Contract

The API must use Keycloak production issuer endpoints:

- `BANANA_KEYCLOAK_ISSUER_URL=https://kc-idp.banana.engineer/realms/banana`
- `BANANA_KEYCLOAK_TOKEN_ISSUER_URL=https://kc-idp.banana.engineer/realms/banana`
- `BANANA_KEYCLOAK_CLIENT_ID=banana-react-spa`

These values are pinned in `src/typescript/api/fly.toml` and validated by `scripts/deploy-api-fly.sh` when `BANANA_DEPLOY_ENV=prod` (default).

## Required prerequisites

1. `fly` CLI is authenticated.
2. Production Neon/Postgres URL is available as one of:
   - `NEON_DATABASE_URL`
   - `DATABASE_URL`
   - `BANANA_PG_CONNECTION`
3. API app secrets already include a JWT signing key (`BANANA_JWT_SECRET`).

## Deploy

From repository root:

```bash
bash scripts/deploy-api-fly.sh
```

Optional parity bypass (not recommended for release deploys):

```bash
BANANA_FLY_SKIP_PARITY_GATE=true bash scripts/deploy-api-fly.sh
```

## Post-deploy checks

1. Health endpoint:

```bash
curl -fsS https://banana-api.fly.dev/health
```

2. Release status:

```bash
fly status -a banana-api
```

3. Rollback reference:

```bash
fly releases -a banana-api
```

## Notes

- Local development continues via Docker Compose.
- Production API auth must never point at localhost issuer URLs.
- Keycloak production deployment is documented in `docs/keycloak/fly-deployment.md`.
