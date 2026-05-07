# Feature Specification: Database Performance Monitoring (150)

**Feature Branch**: `feature/150-database-performance-monitoring`
**Created**: 2026-05-02
**Status**: Ready for implementation
**Input**: PostgreSQL connection (spec 125); monitor query performance
**Prerequisites**: Spec 125 (PostgreSQL via Railway); live database connection
**Blockers**: [INFRASTRUCTURE] Requires live PostgreSQL instance with production query patterns
## Problem Statement

Feature Specification: Database Performance Monitoring (150) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Enable PostgreSQL query logging and slow-query detection.
- Create dashboard to visualize query latency, index usage, and table sizes.
- Integrate slow-query alerts into monitoring (email or Slack).
- Document database performance best practices and optimization procedures.

## Out of Scope

- Automatic query optimization or index creation.
- Historical trend analysis or ML-based prediction.

## Success Criteria

```bash
rails dbconsole  # or psql directly
SELECT query, calls, mean_time FROM pg_stat_statements ORDER BY mean_time DESC LIMIT 10;
python scripts/check-db-performance.py  # Queries slow-query log
python scripts/validate-spec-tasks-parity.py .specify/specs/150-database-performance-monitoring
```
