# Plan: API Rate Limit Verification and Testing (149)

## Design Phase

### Rate Limit Scenarios

1. **Per-IP**: Global limit per client IP (e.g., 1000 req/min).
2. **Per-User**: Authenticated user limit (e.g., 5000 req/min).
3. **Per-Endpoint**: Specific endpoint limits (e.g., `/classify` = 100 req/min).

### Test Strategy

- **Unit tests**: mock HTTP layer, verify rate limit middleware rejects requests.
- **Integration tests**: make real HTTP calls, verify 429 responses.
- **Load tests** (optional): sustained requests to confirm limits hold.

## Success Criteria

- All rate limit tests pass.
- CI enforces rate limit tests before deployment.
- Rate limits prevent abuse without blocking legitimate traffic.
