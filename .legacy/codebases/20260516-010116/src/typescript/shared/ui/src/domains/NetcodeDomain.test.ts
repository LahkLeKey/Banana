import { beforeEach, describe, expect, it } from "bun:test";

import type { InputAggregator } from "./InputDomain";
import type { InteractionManager } from "./InteractionDomain";
import { NetcodeDomain, type NetcodeDomainConfig } from "./NetcodeDomain";

describe("NetcodeDomain", () => {
  let inputAggregator: InputAggregator;
  let interactionManager: InteractionManager;
  let netcode: NetcodeDomain;

  beforeEach(() => {
    inputAggregator = {
      computeMovement: () => ({ moveX: 1, moveZ: 0, source: "keyboard" }),
    } as unknown as InputAggregator;
    interactionManager = {
      getMenuState: () => ({
        visible: false,
        x: 0,
        y: 0,
        normalizedX: 0,
        normalizedY: 0,
      }),
    } as unknown as InteractionManager;
    const config: NetcodeDomainConfig = { tickRate: 60, maxPrediction: 5 };
    netcode = new NetcodeDomain(config, inputAggregator, interactionManager);
  });

  it("advances tick and buffers frames", () => {
    netcode.advanceTick();
    expect(netcode.getCurrentTick()).toBe(1);
    expect(netcode.getFrameBuffer().length).toBe(1);
    expect(netcode.getFrameBuffer()[0].input.moveX).toBe(1);
    expect(netcode.getFrameBuffer()[0].input.moveZ).toBe(0);
  });

  it("limits frame buffer to maxPrediction", () => {
    for (let i = 0; i < 10; i++) netcode.advanceTick();
    expect(netcode.getFrameBuffer().length).toBe(5);
    expect(netcode.getCurrentTick()).toBe(10);
  });

  it("reconciles authoritative state", () => {
    netcode.advanceTick();
    netcode.reconcileAuthoritativeState(1, {
      tickIndex: 1,
      moveX: 1,
      moveZ: 0,
      timestamp: Date.now(),
      isAuthoritative: true,
    });
    expect(netcode.getFrameBuffer()[0].authoritativeState?.isAuthoritative).toBe(true);
  });
});
