# Banana under $20 runbook: Fly-only API + pg_durable Postgres volume

## Goal

Keep the stack Fly-only while staying near or under $20/month:
- `banana-api` app on Fly
- one low-cost Postgres 17 instance on Fly with a single volume
- `pg_durable` installed so durable controller-training workflows can run close to data
- API wired via Banana DB alias contract

## Reality check on budget

This can be done cheaply, but final cost depends on:
- machine size chosen for Postgres
- attached volume size
- backup/snapshot usage
- egress and traffic patterns

Start minimal and scale only if needed.

## Target architecture

- App service: `banana-api` (already exists)
- DB service: `banana-db` (new)
- Region: `iad` (match API primary region)
- API env aliases (all same URL):
  - `NEON_DATABASE_URL`
  - `DATABASE_URL`
  - `BANANA_PG_CONNECTION`

## 1) Create Fly Postgres app (minimal)

From repo root terminal:

```bash
fly apps create banana-db
```

Build and push the repo-owned Fly image with `pg_durable` baked in:

```bash
bash scripts/build-fly-pg-durable-image.sh
```

Override image version/tag when needed:

```bash
PG_DURABLE_VERSION=0.2.2 FLY_DB_IMAGE_TAG=pg17-durable-v0.2.2 \
  bash scripts/build-fly-pg-durable-image.sh
```

Provision a small machine and volume (example defaults):

```bash
fly machine run registry.fly.io/banana-db:pg17-durable-v0.2.2 \
  --app banana-db \
  --region iad \
  --name banana-db-1 \
  --vm-size shared-cpu-1x \
  --volume banana_db_data:10 \
  --env POSTGRES_DB=banana \
  --env POSTGRES_USER=banana_app \
  --env POSTGRES_PASSWORD=REPLACE_WITH_STRONG_PASSWORD \
  --port 5432:5432/tcp
```

Notes:
- Keep volume small at first (10GB) for cost control.
- This is single-node and not HA.
- Keep image tags immutable (for example `pg17-durable-v0.2.2`) so rollbacks are deterministic.

## 2) Validate DB machine is healthy

```bash
fly status -a banana-db
fly machine list -a banana-db
fly logs -a banana-db --no-tail | tail -n 80
```

## 3) Build DB connection string

Use Fly private DNS host format:

```text
postgresql://banana_app:REPLACE_WITH_STRONG_PASSWORD@banana-db.internal:5432/banana?sslmode=disable
```

For tighter security later, move to private networking only and rotated credentials.

On first boot, ensure the database creates the extension:

```sql
ALTER SYSTEM SET shared_preload_libraries = 'pg_durable';
ALTER SYSTEM SET pg_durable.worker_role = 'postgres';
SELECT pg_reload_conf();
```

Restart the Fly DB machine once after applying `ALTER SYSTEM`, then run:

```sql
CREATE EXTENSION IF NOT EXISTS pg_durable;
```

If your app role needs durable workflow access, grant usage after bootstrap.

The repo-owned image preserves postgres-flex startup behavior and provides the
`pg_durable` binaries so the SQL steps above can enable the extension safely on
existing Fly volumes.

## 4) Set Banana API Fly secrets

```bash
fly secrets set -a banana-api \
  NEON_DATABASE_URL="postgresql://banana_app:REPLACE_WITH_STRONG_PASSWORD@banana-db.internal:5432/banana?sslmode=disable" \
  DATABASE_URL="postgresql://banana_app:REPLACE_WITH_STRONG_PASSWORD@banana-db.internal:5432/banana?sslmode=disable" \
  BANANA_PG_CONNECTION="postgresql://banana_app:REPLACE_WITH_STRONG_PASSWORD@banana-db.internal:5432/banana?sslmode=disable"
```

Banana runtime contract requires these aliases to match when strict mode is enabled.

## 5) Deploy API from repository root

```bash
fly deploy -a banana-api --config src/typescript/api/fly.toml
```

## 6) Smoke tests

```bash
curl -i https://banana-api.fly.dev/health

curl -i -X POST https://banana-api.fly.dev/v1/gameplay/sessions/admissions \
  -H "content-type: application/json" \
  -H "x-player-id: smoke-player" \
  -H "x-idempotency-key: smoke-1" \
  -d '{"requestedRole":"dps"}'

curl -i https://banana-api.fly.dev/v1/player/insights \
  -H "x-player-id: smoke-player"
```

Expected:
- `/health` returns 200
- gameplay and player endpoints return non-timeout responses

## 7) Cost controls (important)

- Keep DB at smallest workable VM size.
- Keep one DB machine and one small volume initially.
- Avoid extra replicas/snapshots until needed.
- Monitor with:

```bash
fly status -a banana-db
fly machine list -a banana-db
```

## 8) Troubleshooting

If API times out:

```bash
fly status -a banana-api
fly logs -a banana-api --no-tail | tail -n 120
```

If API crash says no authoritative DB URL:
- secrets are missing or mismatched
- re-run `fly secrets set` for all 3 aliases with identical value

If DB not reachable from API:
- ensure DB machine is running
- verify host is `banana-db.internal`
- verify user/password/db name in connection string

## 9) Security hardening follow-ups

- Rotate DB password after initial bring-up.
- Restrict DB ingress to Fly private networking only.
- Add backup strategy before production traffic.

## 10) Decision gate

Use this Fly-only path if:
- you want one provider and fast setup
- single-node DB risk is acceptable initially

Switch to external/self-hosted DB later if:
- you need lower long-term cost at higher data sizes
- you need stronger HA/backup controls than this minimal setup
- you want full Neon architecture instead of plain Postgres runtime compatibility
