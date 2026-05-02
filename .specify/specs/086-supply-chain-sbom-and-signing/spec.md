# Feature Specification: Supply-Chain Hardening (SBOM + Signed Releases)

**Feature Branch**: `086-supply-chain-sbom-and-signing`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: third
**Domain**: infra / workflow
**Depends on**: #084

## Problem Statement

Container images and shipped binaries have no SBOM, no provenance attestation, and no signature. Downstream consumers cannot verify what they ran.

## In Scope *(mandatory)*

- Generate CycloneDX SBOMs for every shipped image and binary.
- Sign images with cosign keyless OIDC (GitHub Actions OIDC token).
- Publish SLSA Level 3 provenance attestations.
- Document the verification contract for downstream consumers.

## Out of Scope *(mandatory)*

- Custom CA / private signing infra in v1 (keyless OIDC is sufficient).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).

## Status: Implemented

Implemented on branch `sprint3-074-084-086` (May 2026).

- `.github/workflows/sbom-and-signing.yml`: Syft CycloneDX SBOM generation for TS API and ASP.NET API; cosign keyless OIDC image signing on `main`.
- `scripts/generate-sboms.sh`: local developer SBOM generation script.
- `.gitignore`: `artifacts/sboms/` added to prevent committing generated SBOM outputs.
