# Readiness Packets: ML Brain Domain SPIKE

**Date**: 2026-04-26  
**Purpose**: Consolidated bundle of all three domain follow-up readiness packets.

## Selection Summary

The SPIKE has produced one bounded, implementation-ready follow-up slice per brain domain. Each packet is independent — any domain can be executed first without depending on another domain's implementation being complete.

---

## Packet 1: Left Brain — Regression Ripeness Scorer

**Source**: [readiness-packet-left-brain.md](readiness-packet-left-brain.md)  
**Priority**: P1 — lowest implementation risk; establishes foundation for Right Brain context input  
**Execution Risk**: Low  
**Key First Deliverable**: Regression input feature contract and output range contract

### Bounded Slice Summary

Implement a regression-based ripeness scorer in the native C layer, export via wrapper ABI, test with native lane (output range + clamping), and document the consumer usage pattern.

### Acceptance Summary

- Native lane passes output range and clamping assertions.
- Wrapper ABI is stable under unit test.
- Consumer usage pattern documented in wrapper header.

---

## Packet 2: Right Brain — Binary Is-Banana Classifier

**Source**: [readiness-packet-right-brain.md](readiness-packet-right-brain.md)  
**Priority**: P1 — high production value; core banana vs not-banana decision  
**Execution Risk**: Low-Medium (calibration effort required)  
**Key First Deliverable**: Binary operating point (precision/recall target) and input feature contract

### Bounded Slice Summary

Implement a binary is-banana classifier in the native C layer, include calibration, export via wrapper ABI, test with native lane (class label + negative-sample coverage + operating point), and document the operating point configuration.

### Acceptance Summary

- Native lane passes class label, probability range, and negative-sample assertions.
- Operating point is configured and documented.
- Calibration step is implemented and tested.

---

## Packet 3: Full Brain — Transformer Contextual Synthesizer

**Source**: [readiness-packet-full-brain.md](readiness-packet-full-brain.md)  
**Priority**: P3 — highest implementation complexity; execute after Left and Right Brain slices are stable  
**Execution Risk**: High (greenfield, compute cost unknown, new test infrastructure required)  
**Key First Deliverable**: Input format contract (context token schema) and compute cost evaluation

### Bounded Slice Summary

Implement an attention-based transformer in the native C layer, starting with input format contract definition and compute-cost evaluation, then implement model + wrapper ABI + new embedding assertion test pattern. Attention-map logging is in scope.

### Acceptance Summary

- Transformer produces stable output across identical-seed inference calls.
- New embedding assertion test pattern passes in native lane.
- Attention-map logging is operational.
- Input format contract is documented and stable.

---

## Recommended Execution Order

1. **Left Brain** (Regression) — establishes calibrated score contract
2. **Right Brain** (Binary) — establishes categorical decision contract; optionally consumes Left Brain scores
3. **Full Brain** (Transformer) — synthesizes Left+Right outputs as context; requires stable contracts from both

Each packet is independent but this order reduces integration risk.
