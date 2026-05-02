# Feature Specification: Secrets Management (Vault Integration)

**Feature Branch**: `082-infra-secrets-management-vault`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fourth
**Domain**: infra / api
**Depends on**: none

## Problem Statement

Secrets live in env files and per-developer machines. There is no rotation, no audit, no centralized policy.

## In Scope *(mandatory)*

- Adopt HashiCorp Vault (or equivalent) for runtime secret distribution.
- Replace `BANANA_PG_CONNECTION` and similar env-var contracts with Vault-issued credentials.
- Rotation policy + audit pulled into the operator dashboard (feature 064).
- Local dev keeps `.env` workflow with explicit guard rails.

## Out of Scope *(mandatory)*

- Replacing GitHub Actions secrets surface for CI.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
