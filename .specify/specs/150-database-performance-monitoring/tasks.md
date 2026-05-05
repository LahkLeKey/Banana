# Tasks: Database Performance Monitoring (150)

**Input**: Spec at `.specify/specs/150-database-performance-monitoring/spec.md`
**Prerequisites**: Spec 125 (PostgreSQL via Railway)

- [x] T001 Enable PostgreSQL `pg_stat_statements` extension: `CREATE EXTENSION pg_stat_statements;`
- [x] T002 Configure slow-query logging: set `log_min_duration_statement = 500` (log queries > 500ms).
- [x] T003 Query slow queries: `SELECT query, calls, mean_time FROM pg_stat_statements ORDER BY mean_time DESC LIMIT 10;`
- [x] T004 Create `scripts/check-db-performance.py` to query slow-query table and export CSV.
- [x] T005 Set up dashboard (e.g., Grafana or Datadog) to visualize query latency and table sizes.
- [x] T006 Document performance tuning procedures in `docs/database-optimization.md`.
- [x] T007 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/150-database-performance-monitoring` and confirm `OK`.
