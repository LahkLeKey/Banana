# Plan: API Health Checks and Diagnostic Endpoints (146)

## Design Phase

### Health Check Levels

1. **Basic** (`/health`):
   - Returns 200 if alive.
   - Minimal overhead; safe for high-frequency polling.

2. **Extended** (`/health` with query params):
   - `?depth=database` — includes DB connectivity test.
   - `?depth=dependencies` — tests all external services.

3. **Detailed** (`/diagnostics`):
   - Memory, CPU, request latency percentiles.
   - Deployment info, build timestamp.

### Response Format

```json
{
  "status": "ok",
  "timestamp": "2026-05-02T10:00:00Z",
  "uptime_seconds": 3600,
  "checks": {
    "api": "ok",
    "database": "ok",
    "classifier": "ok"
  }
}
```

## Success Criteria

- Endpoints accessible and respond within 100ms.
- Health check catches database disconnection.
- CI aborts deploy if health check fails post-deployment.
