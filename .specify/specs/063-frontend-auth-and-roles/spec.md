# Feature Specification: Auth & Session/Role-Aware UI

**Feature Branch**: `063-frontend-auth-and-roles`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: fourth
**Domain**: react / api
**Depends on**: #053

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

There is no client authentication surface. Operator-only views (training metrics, neuro-trace, audit) cannot exist because there is nothing to gate them on.

## In Scope *(mandatory)*

- Adopt OIDC PKCE flow against an external IdP (or device-bound local accounts for the Electron channel).
- Add session + role storage in Zustand with secure persistence (httpOnly cookie for web, expo-secure-store for RN).
- Add route guards in the new router (depends on 052) keyed off role claims.
- Mirror the auth contract in the ASP.NET pipeline + Fastify+Prisma TS API (parity governance).

## Out of Scope *(mandatory)*

- Multi-tenant org switcher.
- SAML / SCIM provisioning.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
