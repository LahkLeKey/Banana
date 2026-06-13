import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import type {PersistentWorldOrchestrationDomain} from '../domains/persistent-world-orchestration/index.ts';
import {persistentWorldRevisitPathConflict, persistentWorldRevisitValidationError,} from '../lib/errors/domainErrors.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';

import {createWorldServiceForTests} from './world.test-service.ts';
import {registerWorldRoutes} from './world.ts';

function createMockDomainWithError(error: Error):
    PersistentWorldOrchestrationDomain {
  return {
    contractVersion: 'v1',
    orchestrationPathService: {
      selectPath: (phase) => ({phase, pathId: 'baseline-plus-delta-replay-v1'}),
      assertSharedPath: () => undefined,
    },
    areaStateVersionService: {
      ensure: () => ({
        areaId: 'area-test',
        areaStateVersion: 0,
        appliedDeltaThroughSequence: 0,
        canonicalStateSignature: 'sig0',
      }),
      assertNotStale: () => undefined,
      advance: () => ({
        areaId: 'area-test',
        areaStateVersion: 1,
        appliedDeltaThroughSequence: 1,
        canonicalStateSignature: 'sig1',
      }),
      get: () => null,
      reset: () => undefined,
    },
    replayMergeService: {
      ensureAuthoritativeBaseline: (view) => ({
        baselineId: view.baselineId,
        areaIdentity: view.areaIdentity,
        areaStateVersion: view.stateVersion,
        baselinePayload: {orchestrationPathId: view.orchestrationPathId},
        updatedAt: new Date().toISOString(),
      }),
      applyDelta: () => ({
        areaIdentity: {
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-main',
          chunkX: 0,
          chunkY: 0,
          partitionEpoch: 1,
          identityHash: 'abcdabcdabcdabcd',
          areaId: 'area-test',
        },
        baselineId: '550e8400-e29b-41d4-a716-446655440000',
        areaStateVersion: 1,
        appliedDeltaThroughSequence: 1,
        canonicalStateSignature: 'sig1',
      }),
      replay: () => null,
    },
    retryOrchestrationService: {
      decide: () => ({
        classification: 'recoverable',
        shouldRetry: true,
        retryFingerprint: 'abc',
        maxRetryAttempts: 3,
      }),
    },
    failureContextService: {
      build: () => ({
        failureCode: -11,
        classification: 'recoverable',
        operatorMessage: 'deterministic retry available',
        remediationHint: 'retry_with_same_generation_inputs',
      }),
    },
    revisitBaselineService: {
      getAuthoritativeBaseline: () => {
        throw error;
      },
    },
    continuityPayloadService: {
      validateAndNormalize: (payload) => payload as never,
      canonicalRouteKey:
          (fromVariantId,
           toVariantId) => [fromVariantId, toVariantId].sort().join('::'),
      expectedTransitionSignature: () => 'sig-continuity',
    },
  };
}

async function createApp(domain: PersistentWorldOrchestrationDomain) {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app, {
    persistentWorldOrchestrationDomain: domain,
    worldService: createWorldServiceForTests(),
  });
  await app.ready();
  return app;
}

describe('world replay revisit error mapping', () => {
  test('maps revisit validation failures to 400 validation_error', async () => {
    const app = await createApp(createMockDomainWithError(
        persistentWorldRevisitValidationError({field: 'worldId'})));

    const response = await app.inject({
      method: 'POST',
      url: '/api/world/chunk/replay',
      payload: {
        worldId: 'world-a',
        laneId: 'lane-main',
        chunkX: 0,
        chunkY: 0,
        partitionEpoch: 1,
      },
    });

    expect(response.statusCode).toBe(400);
    expect((response.json() as {error: {code: string}}).error.code)
        .toBe('validation_error');

    await app.close();
  });

  test('maps orchestration path mismatch to 409 conflict', async () => {
    const app = await createApp(
        createMockDomainWithError(persistentWorldRevisitPathConflict({
          initialPathId: 'a',
          revisitPathId: 'b',
        })));

    const response = await app.inject({
      method: 'POST',
      url: '/api/world/chunk/replay',
      payload: {
        worldId: 'world-a',
        laneId: 'lane-main',
        chunkX: 0,
        chunkY: 0,
        partitionEpoch: 1,
      },
    });

    expect(response.statusCode).toBe(409);
    expect((response.json() as {error: {code: string}}).error.code)
        .toBe('conflict');

    await app.close();
  });
});
