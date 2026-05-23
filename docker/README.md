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

## Native-To-Neon Integration (Containerized)

Run the native + API Neon integration lane inside Docker:

```bash
NEON_DATABASE_URL="postgres://..." docker compose --profile integration up --build native-neon-integration
```

This service executes `scripts/run-native-neon-integration.sh` in-container,
which builds native with `BANANA_ENABLE_POSTGRES=ON` and runs the strict
Neon integration test.

Use retry tuning env vars when needed:

- `BANANA_NEON_TEST_RETRY_ATTEMPTS`
- `BANANA_NEON_TEST_RETRY_DELAY_MS`
- `BANANA_NEON_CONNECT_TIMEOUT_MS`
