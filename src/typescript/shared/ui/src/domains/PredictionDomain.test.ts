import { describe, expect, it } from "bun:test";

import { DeterministicLocalSimulator, PredictionDomain } from "./PredictionDomain";

describe("PredictionDomain", () => {
  it("predicts deterministic movement state", () => {
    const domain = new PredictionDomain(new DeterministicLocalSimulator(), 10);
    const first = domain.predict({ moveX: 1, moveZ: 0, source: "keyboard" });
    const second = domain.predict({ moveX: 0, moveZ: 1, source: "touch" });

    expect(first.tickIndex).toBe(1);
    expect(first.stateAfter.positionX).toBe(1);
    expect(first.stateAfter.positionZ).toBe(0);

    expect(second.tickIndex).toBe(2);
    expect(second.stateAfter.positionX).toBe(1);
    expect(second.stateAfter.positionZ).toBe(1);
  });

  it("flags divergence and returns authoritative state", () => {
    const domain = new PredictionDomain(new DeterministicLocalSimulator(), 10);
    domain.predict({ moveX: 1, moveZ: 0, source: "keyboard" });

    const reconciled = domain.reconcile({
      tickIndex: 1,
      positionX: 3,
      positionZ: 1,
      timestamp: Date.now(),
    });

    expect(reconciled.confidence).toBe(0);
    expect(reconciled.divergenceReason).toBe("authoritative_mismatch");
    expect(reconciled.stateAfter.positionX).toBe(3);
    expect(reconciled.stateAfter.positionZ).toBe(1);
  });
});
