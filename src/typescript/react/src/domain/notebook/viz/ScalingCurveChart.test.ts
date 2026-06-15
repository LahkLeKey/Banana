import {describe, expect, test} from 'bun:test';

import {ScalingCurveChart} from './ScalingCurveChart';
import type {K3h4ScalingBenchmarkPayload} from './ScalingCurveChart';

const CANONICAL_PAYLOAD: K3h4ScalingBenchmarkPayload = {
  contractVersion: 1,
  schemaVersion: 1,
  series: [
    {n: 64, k3h4Ns: 1_000, attentionNs: 4_000},
    {n: 256, k3h4Ns: 4_000, attentionNs: 65_000},
    {n: 1_024, k3h4Ns: 16_000, attentionNs: 1_048_000},
    {n: 4_096, k3h4Ns: 64_000, attentionNs: 16_777_000},
    {n: 16_384, k3h4Ns: 256_000, attentionNs: 268_435_000},
  ],
  metadata: {
    calibratedSizes: [64, 256, 1_024, 4_096, 16_384],
    extrapolatedAbove: null,
  },
};

describe('ScalingCurveChart', () => {
  test('is a function (renderable component)', () => {
    expect(typeof ScalingCurveChart).toBe('function');
  });

  test('canonical payload has 5 series entries', () => {
    expect(CANONICAL_PAYLOAD.series).toHaveLength(5);
  });

  test('attentionNs grows faster than k3h4Ns across n', () => {
    const {series} = CANONICAL_PAYLOAD;
    const first = series[0]!;
    const last = series[series.length - 1]!;
    const k3h4Ratio = last.k3h4Ns / first.k3h4Ns;
    const attnRatio = last.attentionNs / first.attentionNs;
    /* Attention must grow at least 10x faster than k3h4 across the n range */
    expect(attnRatio).toBeGreaterThan(k3h4Ratio * 10);
  });

  test('contractVersion is 1', () => {
    expect(CANONICAL_PAYLOAD.contractVersion).toBe(1);
  });

  test('metadata.calibratedSizes matches series n values', () => {
    const nValues = CANONICAL_PAYLOAD.series.map((e) => e.n);
    expect(CANONICAL_PAYLOAD.metadata.calibratedSizes).toEqual(nValues);
  });

  test('metadata.extrapolatedAbove is null for canonical fixture', () => {
    expect(CANONICAL_PAYLOAD.metadata.extrapolatedAbove).toBeNull();
  });
});
