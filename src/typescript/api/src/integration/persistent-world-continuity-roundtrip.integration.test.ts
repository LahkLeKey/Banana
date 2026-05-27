import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

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

function makePayload(checkpointSequence: number, idempotencyKey: string) {
  const continuityService = createContinuityPayloadService();
  const seed = {
    contractVersion: 'v1' as const,
    worldId: '550e8400-e29b-41d4-a716-446655440000',
    laneId: 'lane-primary',
    fromVariantId: 'neo-musa',
    toVariantId: 'metro-crescent',
    routeKey: continuityService.canonicalRouteKey('neo-musa', 'metro-crescent'),
    partitionEpoch: 1,
    chunkX: 12,
    chunkY: 7,
    checkpoint: {
      checkpointId: `checkpoint-${checkpointSequence}`,
      checkpointSequence,
      checkpointContextTag: `district:market-${checkpointSequence}`,
      objectiveCompletionIds: ['objective-greet-warden', 'objective-open-gate'],
      profileState: {
        faction: 'banana-guild',
        sequence: checkpointSequence,
      },
      profileFingerprint: `profile-fingerprint-${checkpointSequence}`,
    },
    transitionSignature: '',
  };

  return {
    continuityPayload: {
      ...seed,
      transitionSignature: continuityService.expectedTransitionSignature(seed),
    },
    baseAreaStateVersion: Math.max(0, checkpointSequence - 1),
    idempotencyKey,
  };
}

describe('US1 continuity round-trip integration', () => {
  test(
      'persists and rehydrates checkpoints deterministically over repeated runs',
      async () => {
        const app = await createApp();

        let areaId = '';
        let canonicalSignature = '';

        for (let iteration = 1; iteration <= 10; iteration++) {
          const commit = await app.inject({
            method: 'POST',
            url: '/api/world/continuity/checkpoint',
            payload:
                makePayload(iteration, `continuity-roundtrip-${iteration}`),
          });

          expect(commit.statusCode).toBe(200);
          const commitBody = commit.json() as {
            areaIdentity: {areaId: string};
            areaStateVersion:
                {areaStateVersion: number; canonicalStateSignature: string};
            continuityPayload: {
              checkpoint:
                  {checkpointSequence: number; checkpointContextTag: string;}
            };
          };

          areaId = commitBody.areaIdentity.areaId;
          canonicalSignature =
              commitBody.areaStateVersion.canonicalStateSignature;
          expect(commitBody.continuityPayload.checkpoint.checkpointSequence)
              .toBe(iteration);
          expect(commitBody.continuityPayload.checkpoint.checkpointContextTag)
              .toBe(`district:market-${iteration}`);
          expect(commitBody.areaStateVersion.areaStateVersion).toBe(iteration);

          const rehydrate = await app.inject({
            method: 'GET',
            url: `/api/world/continuity/checkpoint/${areaId}`,
          });

          expect(rehydrate.statusCode).toBe(200);
          const rehydrateBody = rehydrate.json() as {
            areaIdentity: {areaId: string};
            areaStateVersion:
                {areaStateVersion: number; canonicalStateSignature: string};
            continuityPayload: {
              checkpoint:
                  {checkpointSequence: number; checkpointContextTag: string;}
            };
          };

          expect(rehydrateBody.areaIdentity.areaId).toBe(areaId);
          expect(rehydrateBody.areaStateVersion.areaStateVersion)
              .toBe(iteration);
          expect(rehydrateBody.areaStateVersion.canonicalStateSignature)
              .toBe(canonicalSignature);
          expect(rehydrateBody.continuityPayload.checkpoint.checkpointSequence)
              .toBe(iteration);
          expect(
              rehydrateBody.continuityPayload.checkpoint.checkpointContextTag)
              .toBe(`district:market-${iteration}`);
        }

        const replayState = await app.inject({
          method: 'GET',
          url: `/api/world/state/${areaId}`,
        });

        expect(replayState.statusCode).toBe(200);
        expect((replayState.json() as {
                 areaStateVersion: {canonicalStateSignature: string};
               }).areaStateVersion.canonicalStateSignature)
            .toBe(canonicalSignature);

        await app.close();
      });
});
