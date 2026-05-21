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
