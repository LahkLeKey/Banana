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

## Fly.io deployment

Use `scripts/deploy-api-fly.sh` from the repo root. The helper targets the
existing Fly app `banana-api` and requires this database URL input:

- `NEON_DATABASE_URL`

The helper treats `NEON_DATABASE_URL` as the replacement database connection
for the Fly deployment and syncs it into `NEON_DATABASE_URL`, `DATABASE_URL`,
and `BANANA_PG_CONNECTION` before running `fly deploy`.

`NEON_DATABASE_URL` is stored in Vercel as a sensitive project secret for the
`banana` project. To rotate it later, add the new value with `vercel env add`
or replace the secret through the Vercel project settings, then redeploy.

This repository does not provision Neon itself; it deploys the app against a
self-hosted Neon-backed PostgreSQL endpoint.

Recommended database hostname for the self-hosted Neon instance:

- `db.banana.engineer`

Use that hostname in the connection string that you pass via
`NEON_DATABASE_URL`.

```bash
bash scripts/deploy-api-fly.sh
```

For the database cutover, set `NEON_DATABASE_URL` from your self-hosted Neon
instance before invoking the script.
The deploy helper exits with an error if no replacement connection string is
provided.

The deployed service listens on port `8081` and serves `/health` for Fly health
checks.

## Auth/session endpoints

- `GET /auth/steam/start` starts Steam OpenID login.
- `GET /auth/steam/callback` validates Steam OpenID and issues JWT.
- `GET /auth/session` validates active session state.
- `POST /auth/logout` revokes active session state.

Session persistence uses Neon/PostgreSQL when `NEON_DATABASE_URL` (or
`DATABASE_URL` / `BANANA_PG_CONNECTION`) is present, with a memory fallback for
local/non-strict development.

## Authoritative Neon Runtime Contract

The API now resolves a single authoritative database URL at startup in this
order:

1. `NEON_DATABASE_URL`
2. `DATABASE_URL`
3. `BANANA_PG_CONNECTION`

When any one of these is present, startup syncs all three env aliases to the
same value so route/services/native integrations use one consistent Neon
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