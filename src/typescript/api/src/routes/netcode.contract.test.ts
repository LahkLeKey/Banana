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
      return {snapshots: 1, inspections: 2, trainings: 3, routes: 4, nodeTaps: 5};
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
          {x: 1, y: 0, z: 0, coherence: 90},
          {x: 0, y: 1, z: 0, coherence: 80},
          {x: 0, y: 0, z: 1, coherence: 70},
          {x: -1, y: -1, z: -1, coherence: 60},
        ],
        alignment: 42,
        radialStability: 73,
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
  test('aggregates analytics server-side through native netcode service', async () => {
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
    expect(captured.rewardSignal).toBe(77);
    expect(captured.linkSignal).toBe(66);
    expect(response.json()).toMatchObject({
      reward: {neuralRelevanceScore: 88, projectedRewardXp: 144, rewardTier: 1},
      link: {intel: 9, objectives: 8, player: 7, ops: 6},
      vector: {dimensions: 3, contractStrength: [11, 22, 33, 44]},
      hypersphere: {dimensions: 3, alignment: 42, radialStability: 73},
    });

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
    expect(response.json()).toMatchObject({error: 'Invalid netcode analytics payload'});

    await app.close();
  });
});
