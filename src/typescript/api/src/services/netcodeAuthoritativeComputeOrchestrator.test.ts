import {describe, expect, test} from 'bun:test';

import type {NativeNetcodeService} from './nativeNetcode.ts';
import {createNetcodeAnalyticsAuthoritativeComputeOrchestrator, NetcodeAnalyticsOrchestrationError,} from './netcodeAuthoritativeComputeOrchestrator.ts';

function createFakeNativeNetcodeService(): NativeNetcodeService {
  return {
    async reset(): Promise<void> {
      return;
    },
    async recordNodeTap(): Promise<void> {
      return;
    },
    async recordAction(): Promise<void> {
      return;
    },
    async getLedger() {
      return {
        snapshots: 0,
        inspections: 0,
        trainings: 0,
        routes: 0,
        nodeTaps: 0,
      };
    },
    async buildLearning() {
      return {
        modelConfidence: 0,
        trainingAccuracy: 0,
        policyMomentum: 0,
        nodeWeights: [0, 0, 0, 0] as const,
        recommendedNode: 0,
        recommendedAction: 0,
        xpByAction: [0, 0, 0, 0] as const,
      };
    },
    async buildReward() {
      return {
        neuralRelevanceScore: 88,
        projectedRewardXp: 144,
        rewardTier: 1,
      };
    },
    async buildLink() {
      return {
        intel: 9,
        objectives: 8,
        player: 7,
        ops: 6,
      };
    },
    async buildVector() {
      return {
        dimensions: 3,
        nodeVectors: [[1, 2, 3], [4, 5, 6], [7, 8, 9], [2, 4, 6]],
        contractStrength: [11, 22, 33, 44] as const,
      };
    },
    async buildHypersphere() {
      return {
        dimensions: 3,
        nodes: [
          {
            x: 1,
            y: 0,
            z: 0,
            coherence: 90,
            inradius: 0.5,
            nearestNeighborDistance: 1,
          },
          {
            x: 0,
            y: 1,
            z: 0,
            coherence: 80,
            inradius: 0.4,
            nearestNeighborDistance: 0.8,
          },
          {
            x: 0,
            y: 0,
            z: 1,
            coherence: 70,
            inradius: 0.3,
            nearestNeighborDistance: 0.6,
          },
          {
            x: -1,
            y: -1,
            z: -1,
            coherence: 60,
            inradius: 0.2,
            nearestNeighborDistance: 0.4,
          },
        ],
        alignment: 42,
        radialStability: 73,
        clusterCount: 2,
        vectorCount: 4,
        centers: [
          {clusterId: 0, memberCount: 2, centerQ16: [65536, 32768, 16384]},
          {clusterId: 1, memberCount: 2, centerQ16: [32768, 65536, 16384]},
        ],
        radii: [
          {
            clusterId: 0,
            nearestNeighborDistanceQ16: 49152,
            inscribedRadiusQ16: 24576,
            radiusState: 'ok' as const,
          },
          {
            clusterId: 1,
            nearestNeighborDistanceQ16: 49152,
            inscribedRadiusQ16: 24576,
            radiusState: 'ok' as const,
          },
        ],
        weightedVoronoiScores: [
          {
            vectorId: 0,
            clusterId: 0,
            distanceToCenterQ16: 12288,
            weightedScoreQ16: 32768,
            scoreValidity: 'valid' as const,
          },
        ],
        spectralProxy: [
          {
            clusterId: 0,
            frequencyProxyQ16: 131072,
            amplitudeProxyQ16: 32768,
            spectralState: 'ok' as const,
          },
          {
            clusterId: 1,
            frequencyProxyQ16: 131072,
            amplitudeProxyQ16: 32768,
            spectralState: 'ok' as const,
          },
        ],
        observability: {
          convergenceStatus: 0,
          iterationCount: 3,
          assignmentChangesLastIteration: 0,
          deterministicHash: 123456,
          endiannessDecodePath: 'little-endian' as const,
        },
        envelope: {
          contractVersion: 1,
          byteOrderTag: 0x01020304,
          payloadBytes: 892,
          payloadCrc32: 987654321,
          status: 'ok' as const,
        },
      };
    },
  };
}

describe('netcode authoritative compute orchestrator', () => {
  test(
      'returns server-authoritative analytics with LSP representation',
      async () => {
        const orchestrator =
            createNetcodeAnalyticsAuthoritativeComputeOrchestrator(
                createFakeNativeNetcodeService());

        const result = await orchestrator.compute(
            {
              callDensity: 10,
              questPercent: 20,
              playerLevel: 30,
              comboStreak: 40,
              branchPressure: 50,
              dependencyPulse: 60,
              workflowDepth: 2,
              networkDimensions: 6,
              modelConfidence: 77,
              policyMomentum: 66,
            },
            {enabled: true, cohort: 'all'},
        );

        expect(result.contractVersion).toBe(1);
        expect(result.k3h4.observability.deterministicHash).toBe(123456);
        expect(result.abiLayers).toHaveLength(5);
        expect(result.abiLayers[4]).toMatchObject({
          layer: 'hypersphere',
          contractVersion: 1,
          status: 'ok',
          payloadBytes: 892,
          byteOrderTag: 0x01020304,
          deterministicHash: 123456,
        });
        expect(result.abiLayerCoverage).toMatchObject({
          complete: true,
          completeness: 1,
          presentLayers: [
            'learning',
            'reward',
            'link',
            'vector',
            'hypersphere',
          ],
          missingLayers: [],
        });
        expect(result.abiLayerLedger).toEqual([
          {
            layer: 'learning',
            present: true,
            contractVersion: 1,
            status: 'ok',
            payloadBytes: 0,
            byteOrderTag: 0,
            deterministicHash: 88,
          },
          {
            layer: 'reward',
            present: true,
            contractVersion: 1,
            status: 'ok',
            payloadBytes: 0,
            byteOrderTag: 0,
            deterministicHash: 144,
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
            layer: 'hypersphere',
            present: true,
            contractVersion: 1,
            status: 'ok',
            payloadBytes: 892,
            byteOrderTag: 0x01020304,
            deterministicHash: 123456,
          },
        ]);
        expect(result.lspRepresentation).toMatchObject({
          language: 'netcode.analytics.v1',
          boundedContext: 'netcode',
          aggregate: 'k3h4',
          authority: 'server-native',
          contractVersion: 1,
          deterministicHash: 123456,
          rollout: {enabled: true, cohort: 'all'},
        });
      });

  test(
      'maps native decode failures into deterministic orchestration errors',
      async () => {
        const failingNativeService: NativeNetcodeService = {
          ...createFakeNativeNetcodeService(),
          async buildHypersphere() {
            throw new Error('Native hypersphere payload CRC mismatch');
          },
        };
        const orchestrator =
            createNetcodeAnalyticsAuthoritativeComputeOrchestrator(
                failingNativeService);

        await expect(orchestrator.compute(
                         {
                           callDensity: 10,
                           questPercent: 20,
                           playerLevel: 30,
                           comboStreak: 40,
                           branchPressure: 50,
                           dependencyPulse: 60,
                           workflowDepth: 2,
                           networkDimensions: 6,
                           modelConfidence: 77,
                           policyMomentum: 66,
                         },
                         {enabled: true, cohort: 'all'},
                         ))
            .rejects.toBeInstanceOf(NetcodeAnalyticsOrchestrationError);
      });
});
