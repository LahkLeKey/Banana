# Spec Execution Dashboard (2026-05-25)

This board tracks churn across active specs and highlights what must finish before biome and gameplay-element worldbuilding specs can start.

## Portfolio Snapshot

| Spec | Tasks Completed | Tasks Total | Status | Next Gate |
|------|-----------------|-------------|--------|-----------|
| 001-steamworks-release-readiness | 0 | 0 | Needs task regeneration | Generate or refresh actionable tasks.md |
| 002-implement-updated-constitution | N/A | N/A | Artifact-only/no tasks.md | Define execution tasks or close as governance-complete |
| 003-pbj-pickup-steam-release-flow | 1 | 1 | Complete | Keep as closed baseline |
| 004-native-ui-to-native-viewport | N/A | N/A | Artifact-only/no tasks.md | Generate tasks if still in active scope |
| 005-native-runtime-render-modularization | 23 | 29 | In progress | Complete remaining render modularization tasks |
| 006-api-orchestration-drift-reduction | N/A | N/A | Artifact-only/no tasks.md | Generate tasks or archive as superseded |
| 007-api-mmo-recenter | 0 | 49 | Not started | Start setup and foundational API orchestration tasks |
| 008-steam-launch-gating | 41 | 41 | Complete | Keep as launch-gating baseline and monitor regressions |
| 009-persistent-world-orchestration | 51 | 51 | Complete | Monitor regressions and keep evidence fresh as contracts evolve |
| 010-client-static-mesh-banana-line | 18 | 18 | Complete | Ready for runtime playtest sign-off and merge |
| 011-client-world-assets-demo-scenes | 18 | 18 | Complete | Monitor scene-catalog regressions and refresh evidence when contracts change |

## Biome and Gameplay-Element Readiness Criteria

New worldbuilding specs should start only after these are true:

- Steam launch gating has production-safe verify/deny foundations (spec 008 Phase 2 + US1).
- Persistent-world deterministic orchestration has authoritative baseline and replay core (spec 009 Phase 2 + US1/US2 API core).
- MMO API recenter has at least setup/foundational contracts in place (spec 007 early phases).
- Native runtime render modularization blockers are reduced to non-critical polish items (spec 005 near complete).

## Current Churn Plan

1. Keep spec 011 in maintenance mode: rerun focused scene-catalog validation whenever continent/route contracts change.
2. Continue spec 007 setup/foundational tasks to align API gameplay contracts for biome interactions.
3. Regenerate missing tasks.md where needed (001/002/004/006) so all tracked specs remain executable.
4. Prioritize closure of remaining spec 005 render modularization tasks to reduce native runtime churn.
