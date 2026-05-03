# Feature Specification: Native ABI Versioning + Compatibility Contract

**Feature Branch**: `072-native-abi-versioning-contract`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by 067 cross-domain follow-up planning)
**Wave**: first
**Domain**: native / api-interop
**Depends on**: none

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

The native wrapper exports symbols that the C# interop and downstream consumers depend on, but there is no semver discipline or compatibility CI. A silent ABI break would only surface at runtime.

## In Scope *(mandatory)*

- Stamp `libbanana_native` with a version + ABI hash exported via `banana_native_version()`.
- Add a CI lane that diffs exported symbols + struct sizes against the prior tag.
- Document the deprecation policy + grace period in `.specify/wiki/human-reference/`.
- Update the C# interop to assert the expected ABI version at startup.

## Out of Scope *(mandatory)*

- Cross-platform shared libs beyond Linux x86_64 (handed to feature 074).

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the cross-domain planning batch documented in [`.specify/specs/067-cross-domain-followup-planning/spec.md`](../067-cross-domain-followup-planning/spec.md).
