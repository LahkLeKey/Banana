# Feature Specification: Model Registry + Signed Releases

**Feature Branch**: `079-training-model-registry-signed-releases`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: third
**Domain**: training / infra
**Depends on**: none

## Problem Statement

Models are promoted by copying artifacts. There is no signed manifest, no rollback contract, and no audit of which model served which inference.

## In Scope *(mandatory)*

- Stand up a model registry (lightweight: PG table + content-addressed artifact store on S3 / MinIO).
- Sign release manifests with cosign or minisign; verify on load.
- API records the active model id + manifest hash in every verdict response.
- Operator action 'rollback' surfaces a prior signed manifest atomically.

## Out of Scope *(mandatory)*

- External MLflow / Weights & Biases integration.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
