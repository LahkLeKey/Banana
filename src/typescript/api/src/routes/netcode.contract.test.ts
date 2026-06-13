import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import type {NativeNetcodeService} from '../services/nativeNetcode.ts';

import {registerNetcodeRoutes} from './netcode.ts';

type CapturedArgs = {
  rewardSignal?: number;
  linkSignal?: number;
};

function createFakeNetcode(captured: CapturedArgs): NativeNetcodeService {
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
        snapshots: 1,
        inspections: 2,
        trainings: 3,
        routes: 4,
        nodeTaps: 5
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
    async buildReward(_signalInput, interactionSignal) {
      captured.rewardSignal = interactionSignal;
      return {
        neuralRelevanceScore: 88,
        projectedRewardXp: 144,
        rewardTier: 1,
      };
    },
    async buildLink(input) {
      captured.linkSignal = input.interactionSignal;
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
        nodeVectors: [
          [1, 2, 3],
          [4, 5, 6],
          [7, 8, 9],
          [10, 11, 12],
        ],
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

describe('netcode analytics contract', () => {
  test(
      'aggregates analytics server-side through native netcode service',
      async () => {
        const captured: CapturedArgs = {};
        const app = await createApp(createFakeNetcode(captured));

        const response = await app.inject({
          method: 'POST',
          url: '/api/netcode/analytics',
          payload: {
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
        });

        expect(response.statusCode).toBe(200);
  expect(captured.rewardSignal).toBe(73);
        expect(captured.linkSignal).toBe(66);
        const json = response.json();
        expect(json).toMatchObject({
          contractVersion: 1,
          reward:
              {neuralRelevanceScore: 88, projectedRewardXp: 144, rewardTier: 1},
          link: {intel: 9, objectives: 8, player: 7, ops: 6},
          vector: {dimensions: 3, contractStrength: [11, 22, 33, 44]},
          hypersphere: {dimensions: 3, alignment: 42, radialStability: 73},
          lspRepresentation: {
            language: 'netcode.analytics.v1',
            boundedContext: 'netcode',
            aggregate: 'k3h4',
            authority: 'server-native',
            contractVersion: 1,
          },
        });
        expect(json.k3h4.centers).toEqual(expect.arrayContaining([
          expect.objectContaining({clusterId: 0, memberCount: 2}),
        ]));
        expect(json.k3h4.observability).toMatchObject({
          deterministicHash: 123456
        });
        const payload = response.json() as {
          hypersphere: {
            nodes: Array<{inradius: number; nearestNeighborDistance: number;}>;
          };
        };
        expect(payload.hypersphere.nodes[0]?.inradius).toBe(0.5);
        expect(payload.hypersphere.nodes[0]?.nearestNeighborDistance).toBe(1);

        await app.close();
      });

  test('prefers explicit interactionSignal over blended fallback', async () => {
    const captured: CapturedArgs = {};
    const app = await createApp(createFakeNetcode(captured));

    const response = await app.inject({
      method: 'POST',
      url: '/api/netcode/analytics',
      payload: {
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
        interactionSignal: 91,
      },
    });

    expect(response.statusCode).toBe(200);
    expect(captured.rewardSignal).toBe(91);
    expect(captured.linkSignal).toBe(66);

    await app.close();
  });

  test('rejects invalid analytics payloads', async () => {
    const app = await createApp(createFakeNetcode({}));

    const response = await app.inject({
      method: 'POST',
      url: '/api/netcode/analytics',
      payload: {
        callDensity: 10,
        questPercent: 20,
        comboStreak: 40,
        branchPressure: 50,
        dependencyPulse: 60,
        workflowDepth: 2,
        networkDimensions: 6,
        modelConfidence: 77,
        policyMomentum: 66,
      },
    });

    expect(response.statusCode).toBe(400);
    expect(response.json()).toMatchObject({
      error: 'Invalid netcode analytics payload'
    });

    await app.close();
  });

  test('rejects non-finite analytics payload values', async () => {
    const app = await createApp(createFakeNetcode({}));

    const response = await app.inject({
      method: 'POST',
      url: '/api/netcode/analytics',
      payload: {
        callDensity: Number.NaN,
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
    });

    expect(response.statusCode).toBe(400);
    expect(response.json()).toMatchObject({
      error: 'Invalid netcode analytics payload',
    });

    await app.close();
  });

  test(
      'rejects analytics payloads with out-of-range k3h4 dimensions',
      async () => {
        const app = await createApp(createFakeNetcode({}));

        const response = await app.inject({
          method: 'POST',
          url: '/api/netcode/analytics',
          payload: {
            callDensity: 10,
            questPercent: 20,
            playerLevel: 30,
            comboStreak: 40,
            branchPressure: 50,
            dependencyPulse: 60,
            workflowDepth: 2,
            networkDimensions: 17,
            modelConfidence: 77,
            policyMomentum: 66,
          },
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: 'Invalid netcode analytics payload',
        });

        await app.close();
      });

  test('returns deterministic unsupported-version error payload', async () => {
    const captured: CapturedArgs = {};
    const failingService: NativeNetcodeService = {
      ...createFakeNetcode(captured),
      async buildHypersphere() {
        throw new Error('Unsupported native hypersphere contract version');
      },
    };
    const app = await createApp(failingService);

    const response = await app.inject({
      method: 'POST',
      url: '/api/netcode/analytics',
      payload: {
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
    });

    expect(response.statusCode).toBe(502);
    expect(response.json()).toMatchObject({
      errorCode: 'ERR_UNSUPPORTED_VERSION',
      contractVersion: 1,
      retryable: false,
    });

    await app.close();
  });
});
