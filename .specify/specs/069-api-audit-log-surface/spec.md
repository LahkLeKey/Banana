# Feature Specification: API Audit Log Surface

**Feature Branch**: `069-api-audit-log-surface`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: third
**Domain**: api
**Depends on**: #063

## Problem Statement

Training triggers, model promotions, and operator actions are not auditable. Compliance and incident response need an immutable trail.

## In Scope *(mandatory)*

- Add an `audit_event` table + writer middleware to the TS API.
- Mirror the writer in ASP.NET pipeline.
- Expose a filtered read endpoint gated by the operator role.
- Retention policy + deletion / export contract documented in the wiki.

## Out of Scope *(mandatory)*

- External SIEM integration (provide an export endpoint instead).
- PII redaction tooling beyond field allowlists.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
