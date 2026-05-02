# Feature Specification: PII Classification + Field-Level Redaction

**Feature Branch**: `096-data-pii-classification-and-redaction`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: fourth
**Domain**: api / data
**Depends on**: #069

## Problem Statement

There is no inventory of which columns hold PII, no field-level encryption, and the audit log (069) cannot redact safely without one.

## In Scope *(mandatory)*

- Annotate Prisma + EF schemas with PII classification (`@pii(level=...)`).
- Generate redaction middleware from the annotations; applied to logs + audit + export.
- Field-level encryption for high-sensitivity columns (KMS key from feature 082).
- GDPR right-to-erasure endpoint backed by the inventory.

## Out of Scope *(mandatory)*

- Cross-system PII discovery (only repo-defined schemas in v1).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).

## Status: Implemented
