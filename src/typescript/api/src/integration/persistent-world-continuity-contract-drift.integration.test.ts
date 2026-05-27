import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetBaselineRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/baselineRepository.ts';
import {resetDeltaRepositoryForTests} from '../domains/persistent-world-orchestration/persistence/deltaRepository.ts';
import {resetAreaStateVersionServiceForTests} from '../domains/persistent-world-orchestration/services/areaStateVersionService.ts';
import {createContinuityPayloadService} from '../domains/persistent-world-orchestration/services/continuityPayloadService.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerWorldRoutes} from '../routes/world.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app);
  await app.ready();
  return app;
}

function makePayload() {
  const continuityService = createContinuityPayloadService();
  const seed = {
    contractVersion: 'v1' as const,
    worldId: '550e8400-e29b-41d4-a716-446655440000',
    laneId: 'lane-main',
    fromVariantId: 'neo-musa',
    toVariantId: 'metro-crescent',
    routeKey: continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
    partitionEpoch: 1,
    chunkX: 2,
    chunkY: 3,
    checkpoint: {
      checkpointId: 'checkpoint-a',
      checkpointSequence: 1,
      checkpointContextTag: 'district:market-a',
      objectiveCompletionIds: ['objective-a'],
      profileState: {faction: 'banana-guild'},
      profileFingerprint: 'profile-fp-a',
    },
    transitionSignature: '',
  };

  return {
    continuityPayload: {
      ...seed,
      transitionSignature: continuityService.expectedTransitionSignature(seed),
    },
    baseAreaStateVersion: 0,
    idempotencyKey: 'continuity-drift-0001',
  };
}

describe('US2 continuity contract drift guard integration', () => {
  test('rejects strict-schema drift in continuity payload fields', async () => {
    resetBaselineRepositoryForTests();
    resetDeltaRepositoryForTests();
    resetAreaStateVersionServiceForTests();

    const app = await createApp();
    const payload = makePayload();
    (payload.continuityPayload as unknown as Record<string, unknown>)
        .runtimeSchemaVersion = 'native-v2';

    const response = await app.inject({
      method: 'POST',
      url: '/api/world/continuity/checkpoint',
      payload,
    });

    expect(response.statusCode).toBe(400);
    expect(response.json()).toMatchObject({
      error: {
        code: 'validation_error',
        message: 'persistent_world_continuity_payload_invalid',
      },
    });

    await app.close();
  });

  test(
      'rejects contract version drift while preserving deterministic diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload();
        (payload.continuityPayload as unknown as {
          contractVersion: string
        }).contractVersion = 'v2';

        const first = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        const second = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(first.statusCode).toBe(400);
        expect(second.statusCode).toBe(400);
        expect((first.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_payload_invalid');
        expect((second.json() as {error: {message: string}}).error.message)
            .toBe('persistent_world_continuity_payload_invalid');

        await app.close();
      });
});
