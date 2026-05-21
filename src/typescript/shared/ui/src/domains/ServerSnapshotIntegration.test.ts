import { describe, expect, it } from "bun:test";

import { DeterministicLocalSimulator, PredictionDomain } from "./PredictionDomain";
import { ReplicationDomain } from "./ReplicationDomain";

describe("server snapshot reconciliation", () => {
  it("replays cached client inputs after an authoritative correction", () => {
    const prediction = new PredictionDomain(new DeterministicLocalSimulator(), 10);
    const replication = new ReplicationDomain({ maxHistory: 10 });

    prediction.predict({ moveX: 1, moveZ: 0, source: "keyboard" });
    prediction.predict({ moveX: 1, moveZ: 0, source: "keyboard" });

    const snapshot = {
      tick: 1,
      timestamp: Date.now(),
      isAuthoritative: true as const,
      entities: {
        localPlayer: {
          x: 0,
          z: 0,
        },
      },
    };

    replication.applyAuthoritativeState(snapshot);

    const replay = prediction.reconcileAndReplay({
      tickIndex: 1,
      positionX: 0,
      positionZ: 0,
      timestamp: snapshot.timestamp,
    });

    expect(replication.getLatestState()?.state).toEqual(snapshot);
    expect(replay.authoritative.divergenceReason).toBe("authoritative_mismatch");
    expect(replay.replayed).toHaveLength(1);
    expect(replay.replayed[0].stateAfter.positionX).toBe(1);
    expect(replay.replayed[0].stateAfter.tickIndex).toBe(2);
  });
});
