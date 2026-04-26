# Domain Definition Review Checklist (US1)

**Date**: 2026-04-26  
**Purpose**: Gate to confirm canonical domain definitions are non-overlapping, complete, and mapping-ready.

## Checklist

### Domain Completeness

- [ ] Left Brain definition is present with distinct purpose, inputs, outputs, and decision intent
- [ ] Right Brain definition is present with distinct purpose, inputs, outputs, and decision intent
- [ ] Full Brain definition is present with distinct purpose, inputs, outputs, and decision intent
- [ ] All three definitions use canonical terminology from terminology-rules.md

### Non-Overlap Check

- [ ] Left Brain and Right Brain output types are clearly distinguished (continuous vs categorical)
- [ ] Full Brain does not absorb all capabilities of Left and Right (contextual synthesis is distinct role)
- [ ] No model family is left unmapped to a primary domain

### Primary Mapping Completeness

- [ ] Regression mapped to Left Brain with rationale recorded
- [ ] Binary mapped to Right Brain with rationale recorded
- [ ] Transformer mapped to Full Brain with rationale recorded
- [ ] Secondary collaboration edges are documented separately and do not conflict with primary mapping

### Boundary and Risk Completeness

- [ ] Domain boundaries documented in domain-boundaries.md
- [ ] Out-of-domain edge cases identified
- [ ] Known risks per domain recorded in domain-risk-register.md

### Confidence Gate

- [ ] All definitions are High confidence per confidence-policy.md
- [ ] All primary mappings are High confidence per confidence-policy.md
- [ ] Any planned-grade or medium-confidence entries are explicitly annotated
