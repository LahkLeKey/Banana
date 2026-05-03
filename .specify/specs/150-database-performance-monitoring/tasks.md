# Tasks: Database Performance Monitoring (150)

**Input**: Spec at `.specify/specs/150-database-performance-monitoring/spec.md`
**Prerequisites**: Spec 125 (PostgreSQL via Railway)

- [ ] T001 Enable PostgreSQL `pg_stat_statements` extension: `CREATE EXTENSION pg_stat_statements;`
- [ ] T002 Configure slow-query logging: set `log_min_duration_statement = 500` (log queries > 500ms).
- [ ] T003 Query slow queries: `SELECT query, calls, mean_time FROM pg_stat_statements ORDER BY mean_time DESC LIMIT 10;`
- [ ] T004 Create `scripts/check-db-performance.py` to query slow-query table and export CSV.
- [ ] T005 Set up dashboard (e.g., Grafana or Datadog) to visualize query latency and table sizes.
- [ ] T006 Document performance tuning procedures in `docs/database-optimization.md`.
- [ ] T007 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/150-database-performance-monitoring` and confirm `OK`.
