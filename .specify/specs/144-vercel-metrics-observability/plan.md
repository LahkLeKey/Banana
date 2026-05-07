# Plan: Vercel Metrics API and Observability (144)

## Design Phase

### Available Metrics

From Vercel API, queryable metrics include:
- **Performance**: LCP (Largest Contentful Paint), TTFB (Time to First Byte), response time percentiles.
- **Availability**: uptime %, error rate %.
- **Cache**: cache hit rate, edge vs. origin bandwidth.

### Time Ranges

- `--since 7d` — last 7 days.
- `--since 30d` — last 30 days.
- Custom date ranges via `--from` and `--to`.

### Alerting Thresholds

- **LCP > 3s**: warn, consider optimization.
- **LCP > 4s**: alert, trigger investigation.
- **TTFB > 1s**: investigate origin latency.
- **Error rate > 1%**: alert, page broken.

## Implementation Phase

### CLI Query

```bash
vercel metrics query LCP --since 7d --format json
```

### Python Export Script

```python
# scripts/export-vercel-metrics.py
import subprocess, json, sys

metrics = ["LCP", "TTFB", "error_rate"]
result = {}
for metric in metrics:
    output = subprocess.check_output(
        ["vercel", "metrics", "query", metric, "--since", "7d", "--format", "json"]
    )
    result[metric] = json.loads(output)

print(json.dumps(result, indent=2))
```

## Documentation Phase

- `docs/metrics-report.md`: weekly report template with metric definitions, thresholds, and actions.

## Success Criteria

- Metrics queryable via CLI and API.
- Export script runs without errors.
- Weekly report generated with actionable insights.
