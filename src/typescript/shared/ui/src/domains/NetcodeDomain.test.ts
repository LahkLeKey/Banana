import {beforeEach, describe, expect, it} from 'bun:test';

import type {InputAggregator} from './InputDomain';
import {buildNetcodeAbiLayerLedger, NetcodeDomain, type NetcodeDomainConfig, summarizeNetcodeAbiLayers,} from './NetcodeDomain';

describe('NetcodeDomain', () => {
  let inputAggregator: InputAggregator;
  let netcode: NetcodeDomain;

  beforeEach(() => {
    inputAggregator = {
      computeMovement: () => ({moveX: 1, moveZ: 0, source: 'keyboard'}),
    } as unknown as InputAggregator;
    const config: NetcodeDomainConfig = {tickRate: 60, maxPrediction: 5};
    netcode = new NetcodeDomain(config, inputAggregator);
  });

  it('advances tick and buffers frames', () => {
    netcode.advanceTick();
    expect(netcode.getCurrentTick()).toBe(1);
    expect(netcode.getFrameBuffer().length).toBe(1);
    expect(netcode.getFrameBuffer()[0].input.moveX).toBe(1);
    expect(netcode.getFrameBuffer()[0].input.moveZ).toBe(0);
  });

  it('limits frame buffer to maxPrediction', () => {
    for (let i = 0; i < 10; i++) netcode.advanceTick();
    expect(netcode.getFrameBuffer().length).toBe(5);
    expect(netcode.getCurrentTick()).toBe(10);
  });

  it('reconciles authoritative state', () => {
    netcode.advanceTick();
    netcode.reconcileAuthoritativeState(1, {
      tickIndex: 1,
      moveX: 1,
      moveZ: 0,
      timestamp: Date.now(),
      isAuthoritative: true,
    });
    expect(netcode.getFrameBuffer()[0].authoritativeState?.isAuthoritative)
        .toBe(true);
  });

  it('summarizes ABI layer coverage', () => {
    const coverage = summarizeNetcodeAbiLayers([
      {
        layer: 'learning',
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: 1,
      },
      {
        layer: 'reward',
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: 2,
      },
    ]);

    expect(coverage.complete).toBe(false);
    expect(coverage.completeness).toBe(2 / 5);
    expect(coverage.presentLayers).toEqual(['learning', 'reward']);
    expect(coverage.missingLayers).toEqual([
      'link',
      'vector',
      'k3h4',
    ]);
  });

  it('builds ordered ABI layer ledger with missing placeholders', () => {
    const ledger = buildNetcodeAbiLayerLedger([
      {
        layer: 'learning',
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 8,
        byteOrderTag: 0,
        deterministicHash: 11,
      },
      {
        layer: 'vector',
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 64,
        byteOrderTag: 0,
        deterministicHash: 22,
      },
    ]);

    expect(ledger).toEqual([
      {
        layer: 'learning',
        present: true,
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 8,
        byteOrderTag: 0,
        deterministicHash: 11,
      },
      {
        layer: 'reward',
        present: false,
        contractVersion: 1,
        status: 'missing',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: null,
      },
      {
        layer: 'link',
        present: false,
        contractVersion: 1,
        status: 'missing',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: null,
      },
      {
        layer: 'vector',
        present: true,
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 64,
        byteOrderTag: 0,
        deterministicHash: 22,
      },
      {
        layer: 'k3h4',
        present: false,
        contractVersion: 1,
        status: 'missing',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: null,
      },
    ]);
  });
});
