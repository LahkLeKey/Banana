import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import type {K3h4ScalingBenchmarkStatus} from '../services/k3h4ScalingBenchmark.ts';
import type {K3h4ApplicationOrchestrationLayer} from '../services/k3h4ApplicationOrchestrationLayer.ts';
import type {NativeNetcodeService} from '../services/nativeNetcode.ts';
import {type NetcodeAnalyticsAuthoritativeResult, NetcodeAnalyticsOrchestrationError,} from '../services/netcodeAuthoritativeComputeOrchestrator.ts';

import {registerNetcodeRoutes} from './netcode.ts';

type CapturedArgs = {
  rewardSignal?: number;
  linkSignal?: number;
  k3h4Mode?: string;
  spectralMode?: string;
};

function createCanonicalK3h4Projection():
    NetcodeAnalyticsAuthoritativeResult['k3h4Projection'] {
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
}

function createCanonicalAnalyticsResult(
    rollout: {enabled: true; cohort: string},
    abiLayerEntries: NetcodeAnalyticsAuthoritativeResult['abiLayers'],
    ): NetcodeAnalyticsAuthoritativeResult {
  const k3h4Projection = createCanonicalK3h4Projection();
  return {
    contractVersion: 1,
    reward: {neuralRelevanceScore: 88, projectedRewardXp: 144, rewardTier: 1},
    link: {intel: 9, objectives: 8, player: 7, ops: 6},
    vector: {
      dimensions: 3,
      nodeVectors: [
        [1, 2, 3],
        [4, 5, 6],
        [7, 8, 9],
        [10, 11, 12],
      ],
      contractStrength: [11, 22, 33, 44],
    },
    k3h4Projection,
    k3h4: {
      centers: k3h4Projection.centers,
      radii: k3h4Projection.radii,
      weightedVoronoiScores: k3h4Projection.weightedVoronoiScores,
      spectralProxy: k3h4Projection.spectralProxy,
      observability: k3h4Projection.observability,
    },
    k3h4Runtime: {mode: 'power', spectralActivation: 'affinity-graph'},
    abiLayers: abiLayerEntries,
    abiLayerCoverage: {
      expectedLayers: ['learning', 'reward', 'link', 'vector', 'k3h4'],
      presentLayers: abiLayerEntries.map((entry) => entry.layer),
      missingLayers: ['learning', 'reward', 'link', 'vector', 'k3h4'].filter(
          (layer) => !abiLayerEntries.some((entry) => entry.layer === layer)),
      completeness: abiLayerEntries.length / 5,
      complete: abiLayerEntries.length === 5,
    },
    abiLayerLedger: [
      {
        layer: 'learning',
        present: abiLayerEntries.some((entry) => entry.layer === 'learning'),
        contractVersion: 1,
        status: abiLayerEntries.some((entry) => entry.layer === 'learning') ?
            'ok' :
            'missing',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash:
            abiLayerEntries.some((entry) => entry.layer === 'learning') ? 88 :
                                                                          null,
      },
      {
        layer: 'reward',
        present: abiLayerEntries.some((entry) => entry.layer === 'reward'),
        contractVersion: 1,
        status: abiLayerEntries.some((entry) => entry.layer === 'reward') ?
            'ok' :
            'missing',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash:
            abiLayerEntries.some((entry) => entry.layer === 'reward') ? 144 :
                                                                        null,
      },
      {
        layer: 'link',
        present: abiLayerEntries.some((entry) => entry.layer === 'link'),
        contractVersion: 1,
        status: abiLayerEntries.some((entry) => entry.layer === 'link') ?
            'ok' :
            'missing',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash:
            abiLayerEntries.some((entry) => entry.layer === 'link') ? 30 : null,
      },
      {
        layer: 'vector',
        present: abiLayerEntries.some((entry) => entry.layer === 'vector'),
        contractVersion: 1,
        status: abiLayerEntries.some((entry) => entry.layer === 'vector') ?
            'ok' :
            'missing',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash:
            abiLayerEntries.some((entry) => entry.layer === 'vector') ? 3 :
                                                                        null,
      },
      {
        layer: 'k3h4',
        present: abiLayerEntries.some((entry) => entry.layer === 'k3h4'),
        contractVersion: 1,
        status: abiLayerEntries.some((entry) => entry.layer === 'k3h4') ?
            'ok' :
            'missing',
        payloadBytes: 892,
        byteOrderTag: 0x01020304,
        deterministicHash:
            abiLayerEntries.some((entry) => entry.layer === 'k3h4') ? 123456 :
                                                                      null,
      },
    ],
    lspRepresentation: {
      language: 'netcode.analytics.v1',
      boundedContext: 'netcode',
      aggregate: 'k3h4',
      authority: 'server-native',
      contractVersion: 1,
      deterministicHash: 123456,
      rollout,
    },
  };
}

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
    async buildK3h4(input) {
      captured.k3h4Mode = input.k3h4Mode;
      captured.spectralMode = input.spectralMode;
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

async function createApp(netcodeService: NativeNetcodeService) {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerNetcodeRoutes(app, {netcodeService});
  await app.ready();
  return app;
}

async function createAppWithOrchestrationLayer(
    netcodeService: NativeNetcodeService,
    k3h4ApplicationOrchestrationLayer: K3h4ApplicationOrchestrationLayer,
) {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerNetcodeRoutes(
      app,
      {
        netcodeService,
        k3h4ApplicationOrchestrationLayer,
      },
  );
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
            k3h4Mode: 'power',
            spectralMode: 'affinity-graph',
          },
        });

        expect(response.statusCode).toBe(200);
        expect(captured.rewardSignal).toBe(73);
        expect(captured.linkSignal).toBe(66);
        expect(captured.k3h4Mode).toBe('power');
        expect(captured.spectralMode).toBe('affinity-graph');
        const json = response.json();
        expect(json).toMatchObject({
          contractVersion: 1,
          reward:
              {neuralRelevanceScore: 88, projectedRewardXp: 144, rewardTier: 1},
          link: {intel: 9, objectives: 8, player: 7, ops: 6},
          vector: {dimensions: 3, contractStrength: [11, 22, 33, 44]},
          k3h4Projection: {dimensions: 3, alignment: 42, radialStability: 73},
          k3h4Runtime: {
            mode: 'power',
            spectralActivation: 'affinity-graph',
          },
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
        expect(json.abiLayers).toEqual(expect.arrayContaining([
          expect.objectContaining({
            layer: 'k3h4',
            contractVersion: 1,
            status: 'ok',
            payloadBytes: 892,
            byteOrderTag: 0x01020304,
            deterministicHash: 123456,
          }),
        ]));
        expect(json.abiLayerCoverage).toMatchObject({
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
        expect(json.abiLayerLedger).toEqual([
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
            layer: 'k3h4',
            present: true,
            contractVersion: 1,
            status: 'ok',
            payloadBytes: 892,
            byteOrderTag: 0x01020304,
            deterministicHash: 123456,
          },
        ]);
        const payload = response.json() as {
          k3h4Projection: {
            nodes: Array<{inradius: number; nearestNeighborDistance: number;}>;
          };
        };
        expect(payload.k3h4Projection.nodes[0]?.inradius).toBe(0.5);
        expect(payload.k3h4Projection.nodes[0]?.nearestNeighborDistance)
            .toBe(1);

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

  test('rejects unsupported k3h4 mode and spectral mode values', async () => {
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
        networkDimensions: 6,
        modelConfidence: 77,
        policyMomentum: 66,
        k3h4Mode: 'bad-mode',
        spectralMode: 'bad-spectral',
      },
    });

    expect(response.statusCode).toBe(400);
    expect(response.json()).toMatchObject({
      error: 'Invalid netcode analytics payload',
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
      async buildK3h4() {
        throw new Error('Unsupported native k3h4 contract version');
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

  test('routes k3h4 endpoint through API orchestration layer', async () => {
    const captured: CapturedArgs = {};
    const fallbackService: NativeNetcodeService = {
      ...createFakeNetcode(captured),
      async buildK3h4() {
        throw new Error('k3h4 route should not call native buildK3h4 directly');
      },
    };

    const orchestrationLayer: K3h4ApplicationOrchestrationLayer = {
      async compute() {
        return createCanonicalAnalyticsResult(
            {enabled: true, cohort: 'k3h4-direct-route'},
            [
              {
                layer: 'k3h4',
                contractVersion: 1,
                status: 'ok',
                payloadBytes: 892,
                byteOrderTag: 0x01020304,
                deterministicHash: 123456,
              },
            ],
        );
      },
    };

    const app = await createAppWithOrchestrationLayer(
        fallbackService, orchestrationLayer);

    const response = await app.inject({
      method: 'POST',
      url: '/api/netcode/k3h4',
      payload: {
        callDensity: 10,
        questPercent: 20,
        playerLevel: 30,
        comboStreak: 40,
        branchPressure: 50,
        dependencyPulse: 60,
        workflowDepth: 2,
        neuralRelevanceScore: 88,
        networkDimensions: 6,
        modelConfidence: 77,
        policyMomentum: 66,
        k3h4Mode: 'power',
        spectralMode: 'affinity-graph',
      },
    });

    expect(response.statusCode).toBe(200);
    expect(response.json()).toMatchObject({
      k3h4: {
        dimensions: 3,
        alignment: 42,
        radialStability: 73,
      },
    });

    await app.close();
  });

  test('routes vector endpoint through API orchestration layer', async () => {
    const captured: CapturedArgs = {};
    const fallbackService: NativeNetcodeService = {
      ...createFakeNetcode(captured),
      async buildVector() {
        throw new Error(
            'vector route should not call native buildVector directly');
      },
    };

    const orchestrationLayer: K3h4ApplicationOrchestrationLayer = {
      async compute() {
        return createCanonicalAnalyticsResult(
            {enabled: true, cohort: 'vector-direct-route'},
            [
              {
                layer: 'vector',
                contractVersion: 1,
                status: 'ok',
                payloadBytes: 0,
                byteOrderTag: 0,
                deterministicHash: 3,
              },
            ],
        );
      },
    };

    const app = await createAppWithOrchestrationLayer(
        fallbackService, orchestrationLayer);

    const response = await app.inject({
      method: 'POST',
      url: '/api/netcode/vector',
      payload: {
        callDensity: 10,
        questPercent: 20,
        playerLevel: 30,
        comboStreak: 40,
        branchPressure: 50,
        dependencyPulse: 60,
        workflowDepth: 2,
        neuralRelevanceScore: 88,
        networkDimensions: 6,
        modelConfidence: 77,
        policyMomentum: 66,
        k3h4Mode: 'power',
        spectralMode: 'affinity-graph',
      },
    });

    expect(response.statusCode).toBe(200);
    expect(response.json()).toMatchObject({
      vector: {
        dimensions: 3,
        contractStrength: [11, 22, 33, 44],
      },
    });

    await app.close();
  });

  test(
      'maps orchestration errors on vector endpoint to deterministic 502',
      async () => {
        const fallbackService: NativeNetcodeService = createFakeNetcode({});
        const orchestrationLayer: K3h4ApplicationOrchestrationLayer = {
          async compute() {
            throw new NetcodeAnalyticsOrchestrationError(
                'ERR_BAD_CRC',
                'vector crc mismatch',
                true,
            );
          },
        };

        const app = await createAppWithOrchestrationLayer(
            fallbackService, orchestrationLayer);

        const response = await app.inject({
          method: 'POST',
          url: '/api/netcode/vector',
          payload: {
            callDensity: 10,
            questPercent: 20,
            playerLevel: 30,
            comboStreak: 40,
            branchPressure: 50,
            dependencyPulse: 60,
            workflowDepth: 2,
            neuralRelevanceScore: 88,
            networkDimensions: 6,
            modelConfidence: 77,
            policyMomentum: 66,
          },
        });

        expect(response.statusCode).toBe(502);
        expect(response.json()).toMatchObject({
          errorCode: 'ERR_BAD_CRC',
          contractVersion: 1,
          retryable: true,
          rollout: {enabled: true, cohort: 'vector-direct-route'},
        });

        await app.close();
      });

  test(
      'maps orchestration errors on k3h4 endpoint to deterministic 502',
      async () => {
        const fallbackService: NativeNetcodeService = createFakeNetcode({});
        const orchestrationLayer: K3h4ApplicationOrchestrationLayer = {
          async compute() {
            throw new NetcodeAnalyticsOrchestrationError(
                'ERR_UNSUPPORTED_VERSION',
                'k3h4 unsupported version',
                false,
            );
          },
        };

        const app = await createAppWithOrchestrationLayer(
            fallbackService, orchestrationLayer);

        const response = await app.inject({
          method: 'POST',
          url: '/api/netcode/k3h4',
          payload: {
            callDensity: 10,
            questPercent: 20,
            playerLevel: 30,
            comboStreak: 40,
            branchPressure: 50,
            dependencyPulse: 60,
            workflowDepth: 2,
            neuralRelevanceScore: 88,
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
          rollout: {enabled: true, cohort: 'k3h4-direct-route'},
        });

        await app.close();
      });
});

/* --------------------------------------------------------------------------
 * GET /api/netcode/k3h4/scaling-benchmark contract tests (036)
 * -------------------------------------------------------------------------- */

async function buildScalingBenchmarkApp(
    benchmarkStatus: K3h4ScalingBenchmarkStatus,
        ) {
          const app = Fastify();
          registerFastifyErrorMapper(app);
          await registerRequestContextMiddleware(app);
          await registerNetcodeRoutes(app, {
            netcodeAuthoritativeComputeOrchestrator: undefined,
            k3h4ApplicationOrchestrationLayer: undefined,
            netcodeService: undefined,
            scalingBenchmarkLoader: () => benchmarkStatus,
          });
          return app;
        }

        const CANONICAL_BENCHMARK_RESULT = {
          contractVersion: 1 as const,
          schemaVersion: 1,
          series: [
            {n: 64, k3h4Ns: 1000, attentionNs: 4000},
            {n: 256, k3h4Ns: 4000, attentionNs: 65000},
            {n: 1024, k3h4Ns: 16000, attentionNs: 1048000},
            {n: 4096, k3h4Ns: 64000, attentionNs: 16777000},
            {n: 16384, k3h4Ns: 256000, attentionNs: 268435000},
          ],
          metadata: {
            calibratedSizes: [64, 256, 1024, 4096, 16384],
            extrapolatedAbove: null,
          },
        } as const;

describe('GET /api/netcode/k3h4/scaling-benchmark', () => {
  test('returns 200 with scaling series when artifact is available', async () => {
    const app = await buildScalingBenchmarkApp({
      kind: 'ok',
      result: CANONICAL_BENCHMARK_RESULT,
    });

    const response = await app.inject({
      method: 'GET',
      url: '/api/netcode/k3h4/scaling-benchmark',
    });

    expect(response.statusCode).toBe(200);
    const body = response.json();
    expect(body.contractVersion).toBe(1);
    expect(Array.isArray(body.series)).toBe(true);
    expect(body.series).toHaveLength(5);
    expect(body.series[0]).toMatchObject({n: 64});
    expect(body.series[4]).toMatchObject({n: 16384});
    expect(body.metadata.calibratedSizes).toHaveLength(5);
    expect(body.metadata.extrapolatedAbove).toBeNull();

    await app.close();
  });

  test('returns 404 with benchmark_not_found when artifact is missing', async () => {
    const app = await buildScalingBenchmarkApp({kind: 'not_found'});

    const response = await app.inject({
      method: 'GET',
      url: '/api/netcode/k3h4/scaling-benchmark',
    });

    expect(response.statusCode).toBe(404);
    expect(response.json()).toMatchObject({error: 'benchmark_not_found'});

    await app.close();
  });

  test('returns 503 with benchmark_unavailable when I/O fails', async () => {
    const app = await buildScalingBenchmarkApp({
      kind: 'unavailable',
      reason: 'EACCES: permission denied',
    });

    const response = await app.inject({
      method: 'GET',
      url: '/api/netcode/k3h4/scaling-benchmark',
    });

    expect(response.statusCode).toBe(503);
    const body = response.json();
    expect(body.error).toBe('benchmark_unavailable');
    expect(typeof body.message).toBe('string');

    await app.close();
  });

  test('series entries include n, k3h4Ns, and attentionNs', async () => {
    const app = await buildScalingBenchmarkApp({
      kind: 'ok',
      result: CANONICAL_BENCHMARK_RESULT,
    });

    const response = await app.inject({
      method: 'GET',
      url: '/api/netcode/k3h4/scaling-benchmark',
    });

    const {series} = response.json() as {series: unknown[]};
    for (const entry of series) {
      expect(entry).toMatchObject({
        n: expect.any(Number),
        k3h4Ns: expect.any(Number),
        attentionNs: expect.any(Number),
      });
    }

    await app.close();
  });
});

