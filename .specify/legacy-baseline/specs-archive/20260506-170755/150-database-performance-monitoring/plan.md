# Plan: Database Performance Monitoring (150)

## Design Phase

### Monitoring Layers

1. **Query Logging**: PostgreSQL logs slow queries (> 500ms by default).
2. **Statistics**: `pg_stat_statements` tracks query call counts and average latency.
3. **Alerts**: Trigger notification if query time increases or index is missing.

### Key Metrics

- **Query latency**: p50, p95, p99 (milliseconds).
- **Index usage**: identify unused indexes; drop if safe.
- **Table sizes**: GB per table; identify bloat.
- **Connection count**: pooling effectiveness.

## Implementation Phase

### PostgreSQL Configuration

```sql
-- Enable slow query logging
ALTER SYSTEM SET log_min_duration_statement = 500;  -- Log queries > 500ms
ALTER SYSTEM SET log_statement = 'all';  -- Log all statements

-- Create pg_stat_statements extension
CREATE EXTENSION IF NOT EXISTS pg_stat_statements;
SELECT query, calls, mean_time FROM pg_stat_statements ORDER BY mean_time DESC;
```

### Python Export Script

```python
# scripts/check-db-performance.py
import psycopg2, csv, os

conn = psycopg2.connect(os.getenv('BANANA_PG_CONNECTION'))
cur = conn.cursor()

# Query slowest queries
cur.execute("""
  SELECT query, calls, mean_time, max_time
  FROM pg_stat_statements
  ORDER BY mean_time DESC
  LIMIT 20
""")

with open('/tmp/slow-queries.csv', 'w') as f:
    writer = csv.writer(f)
    writer.writerow(['Query', 'Calls', 'Mean (ms)', 'Max (ms)'])
    for row in cur.fetchall():
        writer.writerow(row)

print("Slow queries exported to /tmp/slow-queries.csv")
```

## Monitoring Phase

- Weekly review of slow-query report.
- Investigate top 5 slowest queries.
- Optimize via indexes, query rewrite, or caching.

## Documentation Phase

- `docs/database-optimization.md`: tuning procedures, common slow-query patterns, optimization examples.

## Success Criteria

- Slow-query logging enabled on production database.
- Performance metrics accessible to engineering team.
- Documented procedures for investigating and fixing slow queries.
