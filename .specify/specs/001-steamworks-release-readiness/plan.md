# Implementation Plan: Class Builds, Gear Upgrades, and Skill Combinations

**Branch**: `001-steamworks-release-readiness` | **Date**: 2026-05-21 | **Spec**: `.specify/specs/001-steamworks-release-readiness/spec.md`

**Input**: Feature specification from `.specify/specs/001-steamworks-release-readiness/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/plan-template.md` for the execution workflow.

## Summary

Deliver a gameplay system slice that supports class-driven character builds,
gear progression with meaningful tradeoffs, and deterministic skill-combo logic
for both solo and party-based runs. Implementation prioritizes correctness of
core simulation rules, cross-layer contract consistency, and measurable balance
guardrails before feature expansion.

## Technical Context

<!--
  ACTION REQUIRED: Replace the content in this section with the technical details
  for the project. The structure here is presented in advisory capacity to guide
  the iteration process.
-->

**Language/Version**: Native C (simulation/runtime), TypeScript (API + client), markdown specs

**Primary Dependencies**: Native engine systems, TypeScript API pipeline, client runtime/UI bindings, existing test harnesses

**Storage**: Runtime state + configuration assets (N/A for new persistent schema in this planning slice)

**Testing**: Native unit tests, API integration checks, gameplay scenario validation (solo + party), balance regression suite

**Target Platform**: Windows 10+ primary, containerized runtime channels for reproducible validation

**Project Type**: Cross-domain gameplay systems feature (engine + API + clients)

**Performance Goals**: Deterministic combo evaluation and no critical frame-time regressions in baseline combat scenarios

**Constraints**: Preserve controller -> service -> pipeline -> native interop contracts; keep changes scoped to gameplay systems

**Scale/Scope**: Initial archetype/build/gear/combo slice sufficient for solo and party validation

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [x] Player trust and disclosure alignment verified for gameplay claims (class roles,
  gear impact, combo behavior).
- [x] Cross-domain contracts mapped across native simulation, API payloads, and client
  runtime consumption.
- [x] Quality gates defined for deterministic outcomes, compatibility constraints,
  and failure-path handling in solo and party contexts.
- [x] Reproducible delivery path identified with evidence outputs for gameplay
  validation across supported runtime channels.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/001-steamworks-release-readiness/
├── spec.md
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
└── tasks.md
```

### Source Code (repository root)
<!--
  ACTION REQUIRED: Replace the placeholder tree below with the concrete layout
  for this feature. Delete unused options and expand the chosen structure with
  real paths (e.g., apps/admin, packages/something). The delivered plan must
  not include Option labels.
-->

```text
src/native/
├── engine/
├── include/
└── scaffold/

src/typescript/api/
└── src/

src/typescript/react/
└── src/

tests/native/
tests/
```

**Structure Decision**: Cross-domain implementation through existing native engine,
API, and frontend session-management layers, with gameplay rules centered in native simulation
and surfaced through API/client contracts.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

No constitutional violations currently identified; if a deterministic rule cannot be
represented without contract change, the exception will be logged with explicit
migration tasks.

## Execution Progress (2026-05-21)

- Completed: native gameplay build/combo module integration and tests.
- Completed: API gameplay route contract implementation and validation hardening.
- Completed: React Build Lab integration and UI/test coverage.
- Completed: local visual gameplay verification through session-room -> game-engine flow.

## Runtime Notes

- Docker compose channel may require script line-ending normalization before bootstrap execution in Linux containers.
- Local fallback runtime channel is validated for iterative gameplay/UAT preparation when compose runtime is unstable:
  - API: `BANANA_ENGINE_ADAPTER=inmemory bun run dev` in `src/typescript/api`.
  - React: `VITE_BANANA_API_BASE_URL=http://localhost:8081 bun run dev -- --host 0.0.0.0 --port 5173` in `src/typescript/react`.

## Next Phase: Steam/UAT Readiness

- Produce a repeatable UAT checklist using Steam-playable binary criteria (launchability, session join, movement, build actions, combo confirmation, reconnect behavior).
- Package and document runtime channels for testers (local dev fallback + container channel where available).
- Capture evidence artifacts (screenshots/log excerpts/test outputs) mapped to feature requirements and UAT acceptance.
