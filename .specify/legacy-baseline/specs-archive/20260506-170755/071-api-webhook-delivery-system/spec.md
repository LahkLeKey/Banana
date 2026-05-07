# Feature Specification: Outbound Webhook Delivery System

**Feature Branch**: `071-api-webhook-delivery-system`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fourth
**Domain**: api
**Depends on**: #070

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

External systems cannot subscribe to Banana events (verdict produced, model promoted, training failed). Today every integration is a polling client.

## In Scope *(mandatory)*

- Add subscriber + delivery-attempt tables and an admin CRUD surface.
- Sign payloads with HMAC-SHA256 + signing-secret rotation.
- Use the durable job queue (feature 070) for retries with exponential backoff.
- Document the contract (event types, payload schemas, replay endpoint) in the wiki.

## Out of Scope *(mandatory)*

- Inbound webhook receivers.
- Multi-tenant per-subscriber filtering rules (basic event-type filter only in v1).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
