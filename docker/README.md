# Docker V3 Scaffold

## Shared Overworld Runtime

Run the server-authoritative overworld stack:

```bash
docker compose up --build api-overworld react-overworld
```

Inspect live logs:

```bash
docker compose logs -f api-overworld react-overworld
```

Stop services:

```bash
docker compose down
```

Runtime contract notes:

- `api-overworld` runs the authoritative game session service on `:8081`.
- `react-overworld` runs the portal on `:5173` and targets `http://localhost:8081`.
- Session model is single-room overworld (`sessionId=overworld`) with server tick metrics in snapshot payloads.

## Fly Postgres pg_durable Image

Build and push the repo-owned Fly database image with `pg_durable` baked in:

```bash
bash scripts/build-fly-pg-durable-image.sh
```

Image source and init artifacts:

- `docker/postgres-pg-durable.Dockerfile`

Notes:

- The image extends `flyio/postgres-flex:17.2` to preserve Fly Postgres runtime behavior.
- `pg_durable` is installed in the image, then enabled with SQL on the running instance.

## Native-To-Neon Integration (Containerized)

Run the full compose smoke stack, including the repo-managed PostgreSQL service:

```bash
docker compose up --build
```

The default compose graph now includes:

- `banana-postgres` for the integration database contract
- `api-overworld` for the API runtime
- `react-overworld` for the portal
- `native-neon-integration` for the strict native/API Neon smoke lane

If you only want the native + API Neon integration lane inside Docker:

```bash
docker compose up --build native-neon-integration
```

This service executes `scripts/run-native-neon-integration.sh` in-container,
which builds native with `BANANA_ENABLE_POSTGRES=ON` and runs the strict
Neon integration test.

Use retry tuning env vars when needed:

- `BANANA_NEON_TEST_RETRY_ATTEMPTS`
- `BANANA_NEON_TEST_RETRY_DELAY_MS`
- `BANANA_NEON_CONNECT_TIMEOUT_MS`

## Native Coverage CI-Parity Container

Run native coverage in a local container that mirrors CI native coverage dependencies:

```bash
bash scripts/run-native-coverage-ci-container.sh
```

Reuse an already built image for faster iterations:

```bash
bash scripts/run-native-coverage-ci-container.sh --no-build-image --build-dir out/native-coverage-ci-docker --output-dir artifacts/native/coverage-ci-docker
```

Container definition:

- `docker/native-coverage-ci.Dockerfile`
