# Execution Waves

## Wave 0: Foundations
- Scope: 186-200
- Implementation spec: 242-ds-wave0-foundation-implementation
- Status: ready to execute
- Goal: lock static baseline, persistence, fallback, and layout contracts.
- Exit criteria:
  - Every spike has recommendation and follow-up readiness packet.
  - No unresolved blocker in deferred registry without trigger.

## Wave 1: Primary Chart Contract
- Scope: 201-220
- Implementation spec: 243-ds-wave1-chart-contract-implementation
- Status: queued after Wave 0 evidence closeout
- Goal: settle Plotly versus Altair/Vega ownership and host model.
- Exit criteria:
  - Chosen primary chart path is explicit.
  - Security and performance boundaries are defined.

## Wave 2: Runtime-Expanding Options
- Scope: 221-236
- Implementation spec: 244-ds-wave2-runtime-options-implementation
- Status: queued after Wave 1 contract decision
- Goal: evaluate Bokeh and widget/control pathways without scope leakage.
- Exit criteria:
  - Feasibility decisions are explicit (adopt, defer, reject).
  - Accessibility and trust boundaries are defined.

## Wave 3: Specialized Surface Gate
- Scope: 237-240
- Implementation spec: 245-ds-wave3-specialized-surface-implementation
- Status: queued after Wave 2 adopt/defer/reject outcomes
- Goal: decide map, graph, 3D, and custom-canvas inclusion boundaries.
- Exit criteria:
  - Each specialized surface has a clear go/defer/reject decision.
  - Product-boundary rationale is documented.

## Final Gate: Production Readiness
- Scope: 241 and 246
- Implementation spec: 246-ds-prod-release-execution
- Status: queued after Waves 0-3 complete
- Goal: package one DS production go/no-go recommendation.
- Exit criteria:
  - All waves closed with evidence links.
  - Release-readiness packet complete.
