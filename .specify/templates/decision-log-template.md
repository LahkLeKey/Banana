# Drift Realignment Decision Log

Use this template to record retain/defer/split/reject decisions for planning drift findings.

## Context

- Feature: [feature-id]
- Intake reference: [issue-or-request-id]
- Reviewer lane: [primary-owner]

## Decisions

| Timestamp (UTC) | Finding ID | Decision Type | Decision Owner | Follow-up Destination | Rationale |
| --- | --- | --- | --- | --- | --- |
| [YYYY-MM-DDTHH:MM:SSZ] | [DF-001] | [retain\|defer\|split\|reject] | [owner] | [destination or n/a] | [why this decision was made] |

## Guidance

- Record one row per finding.
- Use `defer` or `split` only when a concrete destination is captured.
- Keep rationale actionable and reviewer-friendly.
- Mirror the same decision record to machine-readable jsonl when automation requires it.
