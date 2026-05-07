# Feature Specification: Helm Chart + Kubernetes Manifests

**Feature Branch**: `081-infra-helm-and-k8s-manifests`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: fourth
**Domain**: infra
**Depends on**: none

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Banana ships compose-only. Production-grade deployment requires k8s manifests with proper liveness/readiness, autoscaling, and resource limits.

## In Scope *(mandatory)*

- Author Helm chart covering API (TS + ASP.NET), Postgres operator dep, native job runner, observability sidecars.
- Define liveness/readiness probes + HPA policies aligned with current compose health checks.
- Document the compose-vs-k8s contract; compose stays canonical for local + CI.
- Add a smoke deploy lane against kind / k3d in CI.

## Out of Scope *(mandatory)*

- Service mesh (Istio/Linkerd) selection.
- Multi-cluster topology.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).

## Status: Implemented
