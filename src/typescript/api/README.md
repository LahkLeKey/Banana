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