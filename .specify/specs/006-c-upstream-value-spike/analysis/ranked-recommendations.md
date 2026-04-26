# Ranked Recommendations

Date: 2026-04-26

## Ranked Candidate List

| Rank | Candidate | Composite Score | Primary Value Signal | Primary Risk/Cost Note |
|---:|---|---:|---|---|
| 1 | CAND-003 DAL Runtime Contract Hardening | 4.00 | Prevents false-success DAL responses and clarifies runtime/build misconfiguration handling upstream. | Requires coordinated status contract updates across native + ASP.NET mapping. |
| 2 | CAND-001 Unified Operational JSON Contract Builder | 3.60 | Reduces malformed payload risk and improves controller-side payload stability. | Requires careful backward-compatible payload shape retention. |
| 3 | CAND-005 ABI/Interop Surface Drift Guard Automation | 3.35 | Unlocks safer native feature iteration by catching contract drift early in CI. | Needs robust rule design to avoid noisy false positives. |
| 4 | CAND-002 Durable Operational State + Reset Contract | 3.30 | Improves runtime determinism and reliability for long-lived operational flows. | Higher delivery effort from lifecycle/persistence coordination. |
| 5 | CAND-004 ML Output Contract Parity | 2.75 | Improves cross-model API consistency and client interpretation. | Medium confidence due to broader contract negotiation needs. |
| 6 | CAND-006 Operational Input Intent Parsing Hardening | 2.35 | Raises deterministic behavior for token-sensitive operational endpoints. | Lower dependency unlock compared with higher-ranked candidates. |

## Recommendation Cutline

Top recommendation set for immediate follow-up packetization (US2):

1. CAND-003
2. CAND-001
3. CAND-005

These three provide the best combined value/risk/dependency profile while preserving bounded story scope.
