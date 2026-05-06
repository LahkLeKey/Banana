# Research: ML Brain Domain SPIKE

**Branch**: `009-ml-brain-domain-spike` | **Phase**: 0 | **Date**: 2026-04-26

## R-01: Left/Right/Full domain semantics

**Question**: How should Left Brain and Right Brain be defined with Transformer fixed as Full Brain?

**Decision**:
- Left Brain = continuous estimation, calibration, and magnitude reasoning domain (Regression-first).
- Right Brain = categorical decisioning, threshold boundaries, and discrete classification domain (Binary-first).
- Full Brain = context synthesis and higher-order sequence interpretation domain (Transformer).

**Rationale**: This partition matches model strengths while minimizing overlap in ownership intent.

**Alternatives considered**:
- Left=Binary, Right=Regression: rejected due weaker semantic fit with estimation vs categorization behavior.
- Full Brain split across all families: rejected because user directive fixes Transformer as Full Brain.

## R-02: Primary model-family mapping strategy

**Question**: Should each family map to one primary domain or multiple equal domains?

**Decision**: Enforce one primary domain per family while allowing documented secondary collaboration edges.

**Rationale**: Primary mapping simplifies ownership and follow-up planning while preserving cross-domain nuance.

**Alternatives considered**:
- Multi-primary mapping: rejected due increased ambiguity for task ownership.

## R-03: Comparison matrix dimensions

**Question**: Which normalized dimensions best support follow-up candidate selection?

**Decision**: Use shared dimensions: output determinism, calibration fidelity, classification confidence clarity, contextual reasoning depth, explainability burden, validation complexity.

**Rationale**: These dimensions directly inform value/risk tradeoffs across all three families.

**Alternatives considered**:
- Domain-specific bespoke dimensions only: rejected because cross-family comparison becomes inconsistent.

## R-04: Follow-up readiness packaging

**Question**: What minimum output makes this SPIKE implementation-ready?

**Decision**: Require one readiness packet per brain domain with objective, bounded scope, validation lanes, and contract risk notes.

**Rationale**: Ensures SPIKE converts to immediate execution candidates.

**Alternatives considered**:
- Ranking summary only: rejected due insufficient implementation handoff detail.
