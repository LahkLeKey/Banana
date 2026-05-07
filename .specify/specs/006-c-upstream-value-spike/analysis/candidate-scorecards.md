# Candidate Scorecards

Date: 2026-04-26
Scoring method: `score = 0.35*value_gain + 0.30*risk_reduction + 0.20*dependency_unlock + 0.10*confidence - 0.15*delivery_effort`

## Scorecard Table

| Candidate | value_gain | risk_reduction | dependency_unlock | delivery_effort | confidence | composite_score | ranking_rationale |
|---|---:|---:|---:|---:|---:|---:|---|
| CAND-001 | 4 | 4 | 4 | 2 | 5 | 3.60 | High near-term reliability gain and broad operational contract impact with manageable effort. |
| CAND-002 | 4 | 5 | 3 | 4 | 4 | 3.30 | Strong reliability/risk improvement but requires heavier coordination across native and interop lifecycle controls. |
| CAND-003 | 5 | 5 | 4 | 3 | 4 | 4.00 | Highest value and risk reduction because DAL failures currently blur deploy/runtime signals; moderate effort. |
| CAND-004 | 4 | 3 | 3 | 3 | 3 | 2.75 | Useful consistency improvement, but dependency unlock is moderate and confidence is lower than operational hardening items. |
| CAND-005 | 3 | 4 | 5 | 2 | 4 | 3.35 | Exceptional dependency unlock for future native expansion with relatively low implementation effort. |
| CAND-006 | 3 | 3 | 2 | 2 | 3 | 2.35 | Improves predictability but narrower strategic leverage than DAL/interop contract candidates. |

## Uncertainty Notes

- CAND-002 uncertainty: exact effort depends on whether persistence is process-local, file-backed, or DB-backed.
- CAND-003 uncertainty: final status taxonomy may require minor contract alignment in ASP.NET and TS proxy layers.
- CAND-004 uncertainty: parity can expand scope if model teams request additional telemetry fields.
