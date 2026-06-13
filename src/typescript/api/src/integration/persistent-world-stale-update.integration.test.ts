import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerWorldRoutes} from '../routes/world.ts';
import {createWorldServiceForTests} from '../routes/world.test-service.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app, {worldService: createWorldServiceForTests()});
  await app.ready();
  return app;
}

describe('US2 stale update rejection integration', () => {
  test(
      'rejects stale base area state version after authoritative state advances',
      async () => {
        const app = await createApp();
        const shared = {
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-stale',
          chunkX: 11,
          chunkY: 7,
          partitionEpoch: 3,
        };

        const first = await app.inject({
          method: 'POST',
          url: '/api/world/state/delta',
          payload: {
            ...shared,
            baseAreaStateVersion: 0,
            idempotencyKey: 'k1',
            deltaPayload: {op: 'place', item: 'rock'},
          },
        });
        expect(first.statusCode).toBe(200);

        const stale = await app.inject({
          method: 'POST',
          url: '/api/world/state/delta',
          payload: {
            ...shared,
            baseAreaStateVersion: 0,
            idempotencyKey: 'k2',
            deltaPayload: {op: 'place', item: 'tree'},
          },
        });

        expect(stale.statusCode).toBe(409);
        expect((stale.json() as {error: {code: string}}).error.code)
            .toBe('conflict');

        await app.close();
      });
});
