import {renderHook, waitFor} from '@testing-library/react';
import {describe, expect, it, mock} from 'bun:test';

let analyticsResponse: unknown = null;
let analyticsError: Error|null = null;

mock.module('../../lib/api', () => {
  class NetcodeAnalyticsError extends Error {
    readonly status: number;
    readonly payload: {
      error?: string;
      errorCode?: string;
      message?: string;
      contractVersion?: number;
      retryable?: boolean;
      rollout?: {enabled: boolean; cohort: string;};
    };

    constructor(status: number, payload: {
      error?: string;
      errorCode?: string;
      message?: string;
      contractVersion?: number;
      retryable?: boolean;
      rollout?: {enabled: boolean; cohort: string;};
    }) {
      super(payload.message ?? payload.error ?? `request failed (${status})`);
      this.name = 'NetcodeAnalyticsError';
      this.status = status;
      this.payload = payload;
    }
  }

  return {
    resolveApiBaseUrl: () => 'http://example.test',
    fetchNetcodeLearning: async () => ({
      ledger: {
        snapshots: 1,
        inspections: 2,
        trainings: 3,
        routes: 4,
        nodeTaps: 5,
      },
      learning: {
        modelConfidence: 77,
        trainingAccuracy: 66,
        policyMomentum: 55,
        nodeWeights: [11, 22, 33, 44] as const,
        recommendedNode: 2,
        recommendedAction: 1,
        xpByAction: [4, 5, 6, 7] as const,
      },
    }),
    fetchNetcodeAnalytics: async () => {
      if (analyticsError) {
        throw analyticsError;
      }
      return analyticsResponse;
    },
    NetcodeAnalyticsError,
  };
});

const {useNetcodeSession} = await import('./useNetcodeSession');
const {NetcodeAnalyticsError} = await import('../../lib/api');

describe('useNetcodeSession contract consumer', () => {
  it('maps API-provided k3h4 payload without local recompute', async () => {
    analyticsError = null;
    analyticsResponse = {
      contractVersion: 1,
      reward: {
        neuralRelevanceScore: 91,
        projectedRewardXp: 42,
        rewardTier: 0,
      },
      link: {intel: 9, objectives: 8, player: 7, ops: 6},
      vector: {
        dimensions: 3,
        nodeVectors: [[1, 2, 3], [4, 5, 6], [7, 8, 9], [2, 4, 6]],
        contractStrength: [31, 41, 51, 61] as const,
      },
      k3h4Projection: {
        dimensions: 3,
        nodes: [
          {
            x: 1,
            y: 0,
            z: 0,
            coherence: 90,
            inradius: 0.4,
            nearestNeighborDistance: 0.8
          },
          {
            x: 0,
            y: 1,
            z: 0,
            coherence: 80,
            inradius: 0.3,
            nearestNeighborDistance: 0.7
          },
          {
            x: 0,
            y: 0,
            z: 1,
            coherence: 70,
            inradius: 0.2,
            nearestNeighborDistance: 0.6
          },
          {
            x: -1,
            y: 0,
            z: 0,
            coherence: 60,
            inradius: 0.1,
            nearestNeighborDistance: 0.5
          },
        ],
        alignment: 88,
        radialStability: 77,
      },
      k3h4: {
        centers: [{
          clusterId: 0,
          centerQ16: [65536, 32768],
          memberVectorIds: [1, 2],
          memberCount: 2
        }],
        radii: [{
          clusterId: 0,
          nearestNeighborDistanceQ16: 49152,
          inscribedRadiusQ16: 24576,
          radiusState: 'ok'
        }],
        weightedVoronoiScores: [{
          vectorId: 1,
          clusterId: 0,
          distanceToCenterQ16: 12288,
          weightedScoreQ16: 32768,
          scoreValidity: 'valid'
        }],
        spectralProxy: [{
          clusterId: 0,
          frequencyProxyQ16: 131072,
          amplitudeProxyQ16: 65536,
          spectralState: 'ok'
        }],
        observability: {
          convergenceStatus: 'converged',
          iterationCount: 3,
          assignmentChangesLastIteration: 0,
          scoringValidity: 'valid',
          deterministicHash: 'a1b2c3d4e5',
        },
      },
      k3h4Runtime: {
        mode: 'power',
        spectralActivation: 'affinity-graph',
      },
      abiLayers: [
        {
          layer: 'learning',
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 0,
          byteOrderTag: 0,
          deterministicHash: 91,
        },
        {
          layer: 'reward',
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 0,
          byteOrderTag: 0,
          deterministicHash: 42,
        },
        {
          layer: 'link',
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 0,
          byteOrderTag: 0,
          deterministicHash: 30,
        },
        {
          layer: 'vector',
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 0,
          byteOrderTag: 0,
          deterministicHash: 3,
        },
        {
          layer: 'k3h4',
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 892,
          byteOrderTag: 0x01020304,
          deterministicHash: 'a1b2c3d4e5',
        },
      ],
      abiLayerCoverage: {
        expectedLayers: [
          'learning',
          'reward',
          'link',
          'vector',
          'k3h4',
        ],
        presentLayers: [
          'learning',
          'reward',
          'link',
          'vector',
          'k3h4',
        ],
        missingLayers: [],
        completeness: 1,
        complete: true,
      },
      abiLayerLedger: [
        {
          layer: 'learning',
          present: true,
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 0,
          byteOrderTag: 0,
          deterministicHash: 91,
        },
        {
          layer: 'reward',
          present: true,
          contractVersion: 1,
          status: 'crc-mismatch',
          payloadBytes: 128,
          byteOrderTag: 0,
          deterministicHash: 4242,
        },
        {
          layer: 'link',
          present: true,
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 0,
          byteOrderTag: 0,
          deterministicHash: 30,
        },
        {
          layer: 'vector',
          present: true,
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 0,
          byteOrderTag: 0,
          deterministicHash: 3,
        },
        {
          layer: 'k3h4',
          present: true,
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 892,
          byteOrderTag: 0x01020304,
          deterministicHash: 'a1b2c3d4e5',
        },
      ],
      rollout: {enabled: true, cohort: 'canary'},
    };

    const {result} = renderHook(() => useNetcodeSession({
                                  callDensity: 30,
                                  questPercent: 40,
                                  comboStreak: 2,
                                  branchPressure: 20,
                                  workflowDepth: 2,
                                  playerLevel: 5,
                                  dependencyPulse: 15,
                                  networkDimensions: 8,
                                }));

    await waitFor(() => {
      expect(result.current.k3h4.centers.length).toBe(1);
    });

    expect(result.current.k3h4.centers[0]?.centerQ16).toEqual([
      65536,
      32768,
    ]);
    expect(result.current.k3h4.radii[0]?.inscribedRadiusQ16).toBe(24576);
    expect(result.current.k3h4.observability.convergenceStatus)
        .toBe('converged');
    expect(result.current.k3h4.runtime).toEqual({
      mode: 'power',
      spectralActivation: 'affinity-graph',
    });
    expect(result.current.abiLayers).toHaveLength(5);
    expect(result.current.abiLayers[4]).toMatchObject({
      layer: 'k3h4',
      payloadBytes: 892,
      byteOrderTag: 0x01020304,
    });
    expect(result.current.abiLayerCoverage).toMatchObject({
      complete: true,
      completeness: 1,
      presentLayers: [
        'learning',
        'reward',
        'link',
        'vector',
        'k3h4',
      ],
      missingLayers: [],
    });
    expect(result.current.abiLayerLedger).toEqual([
      {
        layer: 'learning',
        present: true,
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: 91,
      },
      {
        layer: 'reward',
        present: true,
        contractVersion: 1,
        status: 'crc-mismatch',
        payloadBytes: 128,
        byteOrderTag: 0,
        deterministicHash: 4242,
      },
      {
        layer: 'link',
        present: true,
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: 30,
      },
      {
        layer: 'vector',
        present: true,
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: 3,
      },
      {
        layer: 'k3h4',
        present: true,
        contractVersion: 1,
        status: 'ok',
        payloadBytes: 892,
        byteOrderTag: 0x01020304,
        deterministicHash: 'a1b2c3d4e5',
      },
    ]);
    expect(result.current.analyticsAvailability).toMatchObject({
      available: true,
      reason: 'ok',
      rollout: {enabled: true, cohort: 'canary'},
      contractVersion: 1,
    });
  });

  it('rejects analytics payloads that omit authoritative ABI metadata',
     async () => {
       analyticsError = null;
       analyticsResponse = {
         contractVersion: 1,
         reward: {
           neuralRelevanceScore: 72,
           projectedRewardXp: 21,
           rewardTier: 1,
         },
         link: {intel: 4, objectives: 5, player: 6, ops: 7},
         vector: {
           dimensions: 2,
           nodeVectors: [[1, 2], [2, 3], [3, 4], [4, 5]],
           contractStrength: [10, 20, 30, 40] as const,
         },
         k3h4Projection: {
           dimensions: 2,
           nodes: [
             {
               x: 1,
               y: 0,
               z: 0,
               coherence: 80,
               inradius: 0.3,
               nearestNeighborDistance: 0.6,
             },
             {
               x: 0,
               y: 1,
               z: 0,
               coherence: 70,
               inradius: 0.2,
               nearestNeighborDistance: 0.5,
             },
             {
               x: 0,
               y: 0,
               z: 1,
               coherence: 60,
               inradius: 0.1,
               nearestNeighborDistance: 0.4,
             },
             {
               x: -1,
               y: 0,
               z: 0,
               coherence: 50,
               inradius: 0.1,
               nearestNeighborDistance: 0.3,
             },
           ],
           alignment: 66,
           radialStability: 55,
         },
         k3h4: {
           centers: [
             {
               clusterId: 0,
               centerQ16: [32768, 16384],
               memberVectorIds: [1],
               memberCount: 1,
             },
           ],
           radii: [
             {
               clusterId: 0,
               nearestNeighborDistanceQ16: 32768,
               inscribedRadiusQ16: 16384,
               radiusState: 'ok',
             },
           ],
           weightedVoronoiScores: [
             {
               vectorId: 1,
               clusterId: 0,
               distanceToCenterQ16: 8192,
               weightedScoreQ16: 16384,
               scoreValidity: 'valid',
             },
           ],
           spectralProxy: [
             {
               clusterId: 0,
               frequencyProxyQ16: 65536,
               amplitudeProxyQ16: 32768,
               spectralState: 'ok',
             },
           ],
           observability: {
             convergenceStatus: 'converged',
             iterationCount: 2,
             assignmentChangesLastIteration: 0,
             scoringValidity: 'valid',
             deterministicHash: 'f00baa55',
           },
         },
         k3h4Runtime: {
           mode: 'multiplicative',
           spectralActivation: 'disabled',
         },
         abiLayers: [
           {
             layer: 'learning',
             contractVersion: 1,
             status: 'ok',
             payloadBytes: 0,
             byteOrderTag: 0,
             deterministicHash: 72,
           },
           {
             layer: 'reward',
             contractVersion: 1,
             status: 'ok',
             payloadBytes: 0,
             byteOrderTag: 0,
             deterministicHash: 21,
           },
         ],
         rollout: {enabled: true, cohort: 'legacy'},
       };

       const {result} = renderHook(() => useNetcodeSession({
                                     callDensity: 30,
                                     questPercent: 40,
                                     comboStreak: 2,
                                     branchPressure: 20,
                                     workflowDepth: 2,
                                     playerLevel: 5,
                                     dependencyPulse: 15,
                                     networkDimensions: 8,
                                   }));

       await waitFor(() => {
         expect(result.current.analyticsAvailability.available).toBe(false);
       });

       expect(result.current.analyticsAvailability).toMatchObject({
         available: false,
         reason: 'contract-error',
         rollout: {enabled: true, cohort: 'default'},
       });
     });

  it('surfaces rollout-disabled analytics as unavailable state', async () => {
    analyticsResponse = null;
    analyticsError = new NetcodeAnalyticsError(503, {
      error: 'Netcode k3h4 analytics rollout disabled',
      errorCode: 'ERR_ROLLOUT_DISABLED',
      contractVersion: 1,
      retryable: false,
      rollout: {enabled: false, cohort: 'rollback'},
    });

    const {result} = renderHook(() => useNetcodeSession({
                                  callDensity: 30,
                                  questPercent: 40,
                                  comboStreak: 2,
                                  branchPressure: 20,
                                  workflowDepth: 2,
                                  playerLevel: 5,
                                  dependencyPulse: 15,
                                  networkDimensions: 8,
                                }));

    await waitFor(() => {
      expect(result.current.analyticsAvailability.available).toBe(false);
    });

    expect(result.current.analyticsAvailability).toMatchObject({
      available: false,
      reason: 'rollout-disabled',
      errorCode: 'ERR_ROLLOUT_DISABLED',
      contractVersion: 1,
      rollout: {enabled: false, cohort: 'rollback'},
    });
  });
});
