# Feature Specification: CSP + HTTP Security Headers Hardening

**Feature Branch**: `100-security-csp-and-headers-hardening`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: second
**Domain**: frontend / api
**Depends on**: none

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Frontend currently ships without strict Content-Security-Policy. APIs do not set HSTS / X-Frame-Options / Referrer-Policy uniformly.

## In Scope *(mandatory)*

- Author a strict CSP for React + Electron renderer (nonce-based).
- Add helmet/SecurityHeaders middleware to both APIs with identical policy.
- CI lane scans built HTML + API responses with `securityheaders.com`-equivalent assertions.
- Document the per-runtime exemption list.

## Out of Scope *(mandatory)*

- WAF rules at the edge.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).
