# Banana API on Fly.io

This package can be deployed to Fly.io with the bundled `fly.toml` and `Dockerfile`.

## Product delivery focus

- Primary deliverable: Steam Windows desktop client (DirectX12 runtime path).
- Website role: marketing and account entry surface only.
- Shared backend contract: both surfaces orchestrate against the Neon-backed
	authentication/session store managed by this API.

## Local runtime

```bash
cd src/typescript/api
BANANA_ENGINE_ADAPTER=inmemory HOST=0.0.0.0 PORT=8081 bun run dev
```

## Feature 007 API surfaces

Versioned MMO recenter routes are registered under:

- `/v1/gameplay/*` for authoritative gameplay session orchestration.
- `/v1/player/*` for player-facing account/progression/inventory surfaces.

Validation commands:

```bash
cd src/typescript/api
bun test src/routes/v1/gameplay.contract.test.ts src/routes/v1/gameplay.lifecycle.contract.test.ts src/routes/v1/player.account.contract.test.ts src/routes/v1/player.progression-inventory.contract.test.ts src/routes/v1/player.insights.contract.test.ts
bun test src/integration/player-cross-channel-parity.integration.test.ts src/integration/player-insights-usability.integration.test.ts
bash ../../../scripts/validate-api-parity-governance.sh --strict
```

## Fly.io deployment

Use `scripts/deploy-api-fly.sh` from the repo root. The helper targets the
existing Fly app `banana-api` and requires this database URL input:

- `NEON_DATABASE_URL`

The helper treats `NEON_DATABASE_URL` as the replacement database connection
for the Fly deployment and syncs it into `NEON_DATABASE_URL`, `DATABASE_URL`,
and `BANANA_PG_CONNECTION` before running `fly deploy`.

Release-readiness guards enforced by the deploy helper:

- strict parity governance check (`scripts/validate-api-parity-governance.sh --strict`)
- required `fly.toml` health probe path (`/health`)
- required `fly.toml` internal port (`8081`)

If you intentionally need to bypass parity validation for local emergency smoke,
set `BANANA_FLY_SKIP_PARITY_GATE=true` before running deploy.

`NEON_DATABASE_URL` is stored in Vercel as a sensitive project secret for the
`banana` project. To rotate it later, add the new value with `vercel env add`
or replace the secret through the Vercel project settings, then redeploy.

This repository does not provision PostgreSQL itself; it deploys the app
against a self-hosted authoritative PostgreSQL endpoint.

For Fly production, that endpoint can be a Fly-hosted PostgreSQL 17 instance
using the repo-owned image built from `docker/postgres-pg-durable.Dockerfile`
with `pg_durable` installed and `BANANA_PG_CONNECTION` pointing at the Fly
private DNS host.

Recommended database hostname for the self-hosted instance:

- `db.banana.engineer`

Use that hostname in the connection string that you pass via
`NEON_DATABASE_URL`.

If the Fly database is running `pg_durable`, create the extension during
bootstrap and keep the app pointed at the same authoritative URL through all
three aliases.

Build and publish the DB image to Fly registry from repo root:

```bash
bash scripts/build-fly-pg-durable-image.sh
```

```bash
bash scripts/deploy-api-fly.sh
```

For the database cutover, set `NEON_DATABASE_URL` from your self-hosted
PostgreSQL instance before invoking the script.
The deploy helper exits with an error if no replacement connection string is
provided.

The deployed service listens on port `8081` and serves `/health` for Fly health
checks.

Rollback reference:

```bash
fly releases -a banana-api
fly releases rollback <version> -a banana-api
```

## Auth/session endpoints

- `GET /auth/keycloak/start` starts Keycloak OIDC Authorization Code flow.
- `GET /auth/keycloak/callback` exchanges authorization code with Keycloak and issues Banana JWT.
- `GET /auth/session` validates active session state.
- `POST /auth/logout` revokes active session state.

Optional query parameters on `GET /auth/keycloak/start`:

- `returnTo`: browser destination after callback token handoff.
- `provider`: identity provider alias to pass through to Keycloak as `kc_idp_hint` (for example `github`, `google`, `linkedin`).
- `action`: optional Keycloak auth action (`register` for self-service sign-up, `reset-password` for Keycloak password recovery flow).

Keycloak realm management is the source of truth for identity providers, user login policies, and realm-level auth behavior. API auth integration should consume that realm contract rather than duplicating provider-specific logic in this service.

Session persistence uses PostgreSQL when `NEON_DATABASE_URL` (or
`DATABASE_URL` / `BANANA_PG_CONNECTION`) is present, with a memory fallback for
local/non-strict development.

## Authoritative PostgreSQL Runtime Contract

The API now resolves a single authoritative database URL at startup in this
order:

1. `NEON_DATABASE_URL`
2. `DATABASE_URL`
3. `BANANA_PG_CONNECTION`

When any one of these is present, startup syncs all three env aliases to the
same value so route/services/native integrations use one consistent PostgreSQL
endpoint.

Optional runtime controls:

- `BANANA_NATIVE_PGBOUNCER_ENABLED`:
	set to `false` to skip native PgBouncer auto-configuration.
- `BANANA_NATIVE_PGBOUNCER_MODE`:
	defaults to `transaction`.
- `BANANA_NATIVE_PGBOUNCER_POOL_SIZE`:
	defaults to `20`.
- `BANANA_NEON_STRICT`:
	set to `true` to fail startup when Neon/native PgBouncer bootstrap is
	unavailable.

## Native-To-Neon Integration Test

Run the dedicated integration lane from this package (or call the script
directly from repo root):

```bash
cd src/typescript/api
NEON_DATABASE_URL="postgres://..." bun run test:integration:native-neon
```

What this lane validates:

- Builds native with `BANANA_ENABLE_POSTGRES=ON`.
- Boots API database runtime with strict native PgBouncer configuration.
- Executes a real SQL probe (`SELECT 1`) against the configured Neon URL.

Optional flake-control env vars for CI/cold-start scenarios:

- `BANANA_NEON_TEST_RETRY_ATTEMPTS` (default `3`)
- `BANANA_NEON_TEST_RETRY_DELAY_MS` (default `1500`, linear backoff)
- `BANANA_NEON_CONNECT_TIMEOUT_MS` (default `8000`)