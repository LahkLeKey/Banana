# Ranking Rationale and Tie-Break Decisions

Date: 2026-04-26

## Why the Top Three Lead

1. CAND-003 leads because DAL outcome ambiguity currently crosses native, ASP.NET status mapping, and operational runtime diagnosis. The candidate directly reduces release and incident risk while preserving narrow scope.
2. CAND-001 ranks second due to broad operational coverage (`batch`, `harvest`, `truck`, `ripeness`) and lower effort than lifecycle redesign options.
3. CAND-005 ranks third because it unlocks future native delivery velocity by preventing wrapper/interop drift regressions before they land.

## Why CAND-002 Is Below CAND-005

- CAND-002 has stronger direct risk reduction but materially higher delivery effort and coordination cost.
- CAND-005 provides a better near-term dependency unlock with lower execution friction for upcoming stories.

## Tie-Break Outcome

- No strict composite-score ties occurred.
- Predefined tie-break order remains ready for future revisions:
  1. Higher `risk_reduction`
  2. Higher `dependency_unlock`
  3. Lower `delivery_effort`

## Confidence Notes

- Confidence is strongest for CAND-001/CAND-003/CAND-005 because repository evidence is explicit in current source and tests.
- Confidence is lower for CAND-004/CAND-006 where follow-up scope can expand based on consumer contract expectations.
