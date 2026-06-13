import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {createWorldServiceForTests} from '../routes/world.test-service.ts';
import {registerWorldRoutes} from '../routes/world.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app, {worldService: createWorldServiceForTests()});
  await app.ready();
  return app;
}

describe('US2 cross-player parity integration', () => {
  test(
      'later-arriving player receives same authoritative post-merge state within sync window',
      async () => {
        const app = await createApp();
        const shared = {
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-cross-player',
          chunkX: 10,
          chunkY: 4,
          partitionEpoch: 2,
        };

        const playerOne = await app.inject({
          method: 'POST',
          url: '/api/world/state/delta',
          payload: {
            ...shared,
            baseAreaStateVersion: 0,
            idempotencyKey: 'player-one-k1',
            deltaPayload: {op: 'place', item: 'banner'},
          },
        });
        expect(playerOne.statusCode).toBe(200);

        const startMs = Date.now();
        const playerTwo = await app.inject({
          method: 'GET',
          url: `/api/world/state/${(playerOne.json() as {
                                     areaIdentity: {areaId: string}
                                   }).areaIdentity.areaId}`,
        });
        const elapsedMs = Date.now() - startMs;

        expect(playerTwo.statusCode).toBe(200);
        expect(elapsedMs).toBeLessThanOrEqual(1500);

        const p1 = playerOne.json() as {
          areaStateVersion:
              {canonicalStateSignature: string; areaStateVersion: number};
        };
        const p2 = playerTwo.json() as {
          areaStateVersion:
              {canonicalStateSignature: string; areaStateVersion: number};
        };

        expect(p2.areaStateVersion.canonicalStateSignature)
            .toBe(p1.areaStateVersion.canonicalStateSignature);
        expect(p2.areaStateVersion.areaStateVersion)
            .toBe(p1.areaStateVersion.areaStateVersion);

        await app.close();
      });
});
