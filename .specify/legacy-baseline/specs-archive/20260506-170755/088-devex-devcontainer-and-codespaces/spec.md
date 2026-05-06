# Feature Specification: Dev Container + GitHub Codespaces Onboarding

**Feature Branch**: `088-devex-devcontainer-and-codespaces`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 087 additional planning batch)
**Wave**: second
**Domain**: devex / infra
**Depends on**: none

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

New contributors must follow a long onboarding doc to get the toolchain working (Bun, .NET 9, CMake, native libs, Postgres). A devcontainer collapses this to one click.

## In Scope *(mandatory)*

- Author `.devcontainer/devcontainer.json` covering Bun + .NET + CMake + Postgres + native deps.
- Validate it in GitHub Codespaces and locally in VS Code Dev Containers.
- Smoke a sample feature loop (`bun run check` + `dotnet test`) on first attach.
- Document the choice between devcontainer and the native Ubuntu WSL2 contract.

## Out of Scope *(mandatory)*

- Replacing the Ubuntu WSL2 runtime contract — devcontainer is additive.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the additional cross-domain planning batch documented in [`.specify/specs/087-additional-followup-planning/spec.md`](../087-additional-followup-planning/spec.md).

## Status: Implemented
