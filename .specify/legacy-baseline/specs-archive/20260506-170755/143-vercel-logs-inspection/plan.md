# Plan: Vercel Logs Inspection and Analysis (143)

## Design Phase

### Log Sources

Vercel provides logs from:
1. **Build logs**: `bun run build`, Vite compilation, asset bundling.
2. **Runtime logs**: HTTP requests, errors, timeouts (if API is hosted on Vercel).
3. **Infrastructure logs**: DNS, CDN cache hits/misses, SSL provisioning.

### Log Retention

- Vercel retains logs for a limited time (typically 7–30 days).
- For long-term archival, export to GitHub Actions artifact or S3.

## Implementation Phase

### CLI Command

```bash
vercel logs https://banana.engineer
vercel logs --tail  # Real-time tail (if supported)
vercel logs --since "2026-05-01" --until "2026-05-02"  # Date range
```

### Filtering

Common filters:
- `--level=error` — only error messages.
- `--level=warning` — warnings and above.
- `--search "timeout"` — search keyword in logs.

## Documentation Phase

- `docs/runbooks/logs.md`: log format, filtering examples, common errors.
- Debugging patterns: "Build fails at asset bundling", "API timeouts", "Redirect loops", etc.

## Success Criteria

- Logs accessible via CLI.
- Filtering works correctly.
- Runbook contains resolvable debugging steps for 3+ error patterns.
