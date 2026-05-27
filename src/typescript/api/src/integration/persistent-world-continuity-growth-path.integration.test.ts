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

function makePayload(contextTag: string) {
  const continuityService = createContinuityPayloadService();
  const seed = {
    contractVersion: 'v1' as const,
    worldId: '550e8400-e29b-41d4-a716-446655440000',
    laneId: 'lane-growth',
    fromVariantId: 'neo-musa',
    toVariantId: 'metro-crescent',
    routeKey: continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
    partitionEpoch: 1,
    chunkX: 2,
    chunkY: 9,
    checkpoint: {
      checkpointId: 'checkpoint-growth',
      checkpointSequence: 1,
      checkpointContextTag: contextTag,
      objectiveCompletionIds: ['objective-growth'],
      profileState: {faction: 'banana-guild'},
      profileFingerprint: 'profile-growth-a',
    },
    transitionSignature: '',
  };

  return {
    continuityPayload: {
      ...seed,
      transitionSignature: continuityService.expectedTransitionSignature(seed),
    },
    baseAreaStateVersion: 0,
    idempotencyKey: 'continuity-growth-0001',
  };
}

describe('US3 continuity growth-path integration', () => {
  test(
      'new checkpoint context field persists and rehydrates through owning contract path',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload('district:growth-market');

        const commit = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(commit.statusCode).toBe(200);
        const committed = commit.json() as {
          areaIdentity: {areaId: string};
          continuityPayload: {checkpoint: {checkpointContextTag: string};};
        };

        const rehydrate = await app.inject({
          method: 'GET',
          url: `/api/world/continuity/checkpoint/${
              committed.areaIdentity.areaId}`,
        });

        expect(rehydrate.statusCode).toBe(200);
        expect(rehydrate.json()).toMatchObject({
          continuityPayload: {
            checkpoint: {
              checkpointContextTag: 'district:growth-market',
            },
          },
        });

        await app.close();
      });

  test(
      'disabled checkpoint context values emit explicit diagnostics',
      async () => {
        resetBaselineRepositoryForTests();
        resetDeltaRepositoryForTests();
        resetAreaStateVersionServiceForTests();

        const app = await createApp();
        const payload = makePayload('disabled:legacy-zone');

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/continuity/checkpoint',
          payload,
        });

        expect(response.statusCode).toBe(400);
        expect(response.json()).toMatchObject({
          error: {
            code: 'validation_error',
            message:
                'persistent_world_continuity_checkpoint_context_unsupported',
          },
        });

        await app.close();
      });
});
