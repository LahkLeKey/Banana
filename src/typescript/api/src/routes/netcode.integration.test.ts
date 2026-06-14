import {afterEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import type {NativeNetcodeService} from '../services/nativeNetcode.ts';

import {registerNetcodeRoutes} from './netcode.ts';

type EnvSnapshot = Record<string, string|undefined>;

function snapshotEnv(keys: readonly string[]): EnvSnapshot {
  const snapshot: EnvSnapshot = {};
  for (const key of keys) {
    snapshot[key] = process.env[key];
  }
  return snapshot;
}

function restoreEnv(snapshot: EnvSnapshot): void {
  for (const [key, value] of Object.entries(snapshot)) {
    if (value === undefined) {
      delete process.env[key];
      continue;
    }
    process.env[key] = value;
  }
}

function createBaseService(): NativeNetcodeService {
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
        nodeTaps: 0
      };
    },
    async buildLearning() {
      return {
        modelConfidence: 50,
        trainingAccuracy: 60,
        policyMomentum: 70,
        nodeWeights: [1, 2, 3, 4] as const,
        recommendedNode: 2,
        recommendedAction: 1,
        xpByAction: [4, 5, 6, 7] as const,
      };
    },
    async buildReward() {
      return {neuralRelevanceScore: 88, projectedRewardXp: 144, rewardTier: 1};
    },
    async buildLink() {
      return {intel: 9, objectives: 8, player: 7, ops: 6};
    },
    async buildVector() {
      return {
        dimensions: 3,
        nodeVectors: [[1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12]],
        contractStrength: [11, 22, 33, 44] as const,
      };
    },
    async buildK3h4() {
      return {
        dimensions: 3,
        nodes: [
          {
            x: 1,
            y: 0,
            z: 0,
            coherence: 90,
            inradius: 0.5,
            nearestNeighborDistance: 1
          },
          {
            x: 0,
            y: 1,
            z: 0,
            coherence: 80,
            inradius: 0.4,
            nearestNeighborDistance: 0.8
          },
          {
            x: 0,
            y: 0,
            z: 1,
            coherence: 70,
            inradius: 0.3,
            nearestNeighborDistance: 0.6
          },
          {
            x: -1,
            y: -1,
            z: -1,
            coherence: 60,
            inradius: 0.2,
            nearestNeighborDistance: 0.4
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
      };
    },
  };
}

async function createApp(netcodeService: NativeNetcodeService) {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerNetcodeRoutes(app, {netcodeService});
  await app.ready();
  return app;
}

const analyticsPayload = {
  callDensity: 10,
  questPercent: 20,
  playerLevel: 30,
  comboStreak: 40,
  branchPressure: 50,
  dependencyPulse: 60,
  workflowDepth: 2 as const,
  networkDimensions: 6,
  modelConfidence: 77,
  policyMomentum: 66,
};

afterEach(() => {
  delete process.env.BANANA_NETCODE_K3H4_ENABLED;
  delete process.env.BANANA_NETCODE_K3H4_COHORT;
});

describe('netcode integration', () => {
  test(
      'maps CRC mismatch decode failures to deterministic error contract',
      async () => {
        const service: NativeNetcodeService = {
          ...createBaseService(),
          async buildK3h4() {
            throw new Error('Native k3h4 payload CRC mismatch');
          },
        };
        const app = await createApp(service);

        const response = await app.inject({
          method: 'POST',
          url: '/api/netcode/analytics',
          payload: analyticsPayload,
        });

        expect(response.statusCode).toBe(502);
        expect(response.json()).toMatchObject({
          errorCode: 'ERR_BAD_CRC',
          contractVersion: 1,
          retryable: true,
        });

        await app.close();
      });

  test(
      'returns rollout-off fallback payload when feature disabled',
      async () => {
        const envSnapshot = snapshotEnv([
          'BANANA_NETCODE_K3H4_ENABLED',
          'BANANA_NETCODE_K3H4_COHORT',
        ]);
        const app = await createApp(createBaseService());
        try {
          process.env.BANANA_NETCODE_K3H4_ENABLED = 'false';
          process.env.BANANA_NETCODE_K3H4_COHORT = 'rollback';

          const response = await app.inject({
            method: 'POST',
            url: '/api/netcode/analytics',
            payload: analyticsPayload,
          });

          expect(response.statusCode).toBe(503);
          expect(response.json()).toMatchObject({
            error: 'Netcode k3h4 analytics rollout disabled',
            rollout: {enabled: false, cohort: 'rollback'},
          });
        } finally {
          await app.close();
          restoreEnv(envSnapshot);
        }
      });
});
