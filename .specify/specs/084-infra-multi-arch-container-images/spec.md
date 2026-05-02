# Feature Specification: Multi-Architecture Container Images

**Feature Branch**: `084-infra-multi-arch-container-images`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: third
**Domain**: infra
**Depends on**: #074

## Problem Statement

Container images are amd64 only. Apple Silicon developers compile inside Rosetta and arm64 servers are unsupported.

## In Scope *(mandatory)*

- Convert each Dockerfile to multi-arch via `docker buildx`.
- Publish manifests covering linux/amd64 + linux/arm64.
- Update compose health checks + smoke flows to verify both arches.
- Pin base-image digests + record per-arch SBOM (handed to feature 086).

## Out of Scope *(mandatory)*

- windows/amd64 container variants.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
