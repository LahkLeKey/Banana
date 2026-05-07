# Feature Specification: Theming & Dark Mode via Design Tokens

**Feature Branch**: `059-frontend-theming-and-dark-mode`
**Created**: 2026-05-01
**Status**: Stub (scaffolded by feature 051 frontend-robustness spike)
**Wave**: third
**Domain**: shared/ui
**Depends on**: #056

## Success Criteria

- Feature is implemented according to the In Scope section
- All tasks in tasks.md are completed and verified
- Code changes are tested and pass CI/CD gates
- Documentation is updated to reflect new behavior


## Problem Statement

Theming is 'default Tailwind'. The token pipeline emits values but the UI never consumes a non-default theme; dark mode does not exist.

## In Scope *(mandatory)*

- Extend the `tokens/` pipeline to emit CSS-variable themes (light + dark + a high-contrast preset).
- Adopt the shadcn theming convention so existing primitives respect `data-theme`.
- Add a theme toggle component in shared/ui with persistence via Zustand.
- Document the contract in Storybook (depends on 056).

## Out of Scope *(mandatory)*

- Per-tenant or per-customer theme branding.
- Animated theme transitions.

## Notes for the planner

- Run `speckit.specify` against this stub to expand User Scenarios, Functional Requirements, and Success Criteria before `speckit.plan`.
- Cross-layer dependencies (API or native changes) MUST be enumerated in the plan artifact and parity-governed (feature 047) when applicable.
- This stub was generated as part of the 15-feature follow-up inventory in [`.specify/specs/051-frontend-robustness-spike/spec.md`](../051-frontend-robustness-spike/spec.md).
