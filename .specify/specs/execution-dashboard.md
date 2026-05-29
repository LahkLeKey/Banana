# Spec Execution Dashboard (2026-05-25)

This board tracks churn across active specs and highlights what must finish before biome and gameplay-element worldbuilding specs can start.

## Portfolio Snapshot

| Spec | Tasks Completed | Tasks Total | Status | Next Gate |
|------|-----------------|-------------|--------|-----------|
| 001-steamworks-release-readiness | 19 | 22 | In progress | 3 remaining external-platform tasks gated on Steamworks build/upload/playable validation (not code-blocking) |
| 002-implement-updated-constitution | N/A | N/A | Artifact-only/no tasks.md | Governance-complete; no executable tasks expected |
| 003-pbj-pickup-steam-release-flow | 12 | 13 | In progress | Runbook, library-verification checklist, and PR closeout template added; final task gated on next Steam publish cycle (not code-blocking) |
| 004-native-ui-to-native-viewport | N/A | N/A | Artifact-only/no tasks.md | Closed-as-superseded by 005 modularization unless re-scoped |
| 005-native-runtime-render-modularization | 29 | 29 | Complete | All US1/US2 seam coverage landed; evidence in artifacts/native/005-us1-runtime-tick.txt and artifacts/native/005-us2-dx12-diagnostics.txt |
| 006-api-orchestration-drift-reduction | N/A | N/A | Artifact-only/no tasks.md | Closed-as-superseded by 007 API MMO Recenter unless re-scoped |
| 007-api-mmo-recenter | 49 | 49 | Complete | All 49 tasks landed across US1/US2/US3 setup, foundational, and polish phases |
| 008-steam-launch-gating | 41 | 41 | Complete | Keep as launch-gating baseline and monitor regressions |
| 009-persistent-world-orchestration | 51 | 51 | Complete | Monitor regressions and keep evidence fresh as contracts evolve |
| 010-client-static-mesh-banana-line | 18 | 18 | Complete | Ready for runtime playtest sign-off and merge |
| 011-client-world-assets-demo-scenes | 18 | 18 | Complete | Monitor scene-catalog regressions and refresh evidence when contracts change |
| 012-c3-integrations | N/A | N/A | Artifact-only/no tasks.md | Closed-as-superseded by 013 native-c3-orchestration-migration unless re-scoped |
| 013-native-c3-orchestration-migration | N/A | N/A | Artifact-only/no tasks.md | Closed-as-superseded by 015+ coherent-world suite unless re-scoped |
| 014-add-gameplay-assets | 30 | 30 | Complete | All US1/US2/US3 catalog growth + diagnostics coverage landed; evidence in artifacts/native/014-add-gameplay-assets/ |
| 015-coherent-game-world | 27 | 27 | Superseded-by-031 | Folded into 031-unified-coherent-world umbrella |
| 016-gameplay-api-continuity | 24 | 24 | Superseded-by-031 | Folded into 031-unified-coherent-world umbrella |
| 017–030 coherent/cohesion lane slices | 16 specs | 24 each | Superseded-by-031 | All 16 cohesion-lane specs folded into 031-unified-coherent-world; no further per-lane specs allowed |
| 031-unified-coherent-world | 14 | 14 | Complete | Canonical unified slice is green and consolidated: CI guardrail (`scripts/validate-coherent-world-unified-slice.sh`) is wired in `.github/workflows/banana.yml`; one-pass native evidence refresh (`scripts/refresh-coherent-world-evidence.sh`) regenerated umbrella 29/29 (4.60s), full native 107/107 (14.77s), and full-MMO demo 1/1; API baseline refresh (`scripts/refresh-coherent-world-api-baselines.sh`) refreshed API unit (14/0 stable subset) and integration (45/0 across 14 files) evidence. |

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
