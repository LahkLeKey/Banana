# Banana API on Fly.io

This package can be deployed to Fly.io with the bundled `fly.toml` and `Dockerfile`.

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

This repository does not provision Neon itself; it deploys the app against a
self-hosted Neon-backed PostgreSQL endpoint.

```bash
bash scripts/deploy-api-fly.sh
```

For the database cutover, set `NEON_DATABASE_URL` from your self-hosted Neon
instance before invoking the script.
The deploy helper exits with an error if no replacement connection string is
provided.

The deployed service listens on port `8081` and serves `/health` for Fly health
checks.