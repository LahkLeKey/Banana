import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerWorldRoutes} from '../routes/world.ts';

type ReplayResponse = {
  baselineId: string; orchestrationPathId: string;
  areaIdentity: {areaId: string; identityHash: string;};
  areaStateVersion: {canonicalStateSignature: string;};
};

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app);
  await app.ready();
  return app;
}

describe('US1 contract-path parity integration', () => {
  test(
      'initial and revisit requests use shared orchestration path contract',
      async () => {
        const app = await createApp();
        const payload = {
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'primary',
          chunkX: 9,
          chunkY: 12,
          partitionEpoch: 4,
        };

        const initialReplay = await app.inject({
          method: 'POST',
          url: '/api/world/chunk/replay',
          payload,
        });
        expect(initialReplay.statusCode).toBe(200);
        const initialBody = initialReplay.json() as ReplayResponse;

        const revisitReplay = await app.inject({
          method: 'POST',
          url: '/api/world/chunk/replay',
          payload,
        });
        expect(revisitReplay.statusCode).toBe(200);
        const revisitBody = revisitReplay.json() as ReplayResponse;

        expect(revisitBody.orchestrationPathId)
            .toBe(initialBody.orchestrationPathId);
        expect(revisitBody.baselineId).toBe(initialBody.baselineId);
        expect(revisitBody.areaIdentity.areaId)
            .toBe(initialBody.areaIdentity.areaId);
        expect(revisitBody.areaIdentity.identityHash)
            .toBe(initialBody.areaIdentity.identityHash);
        expect(revisitBody.areaStateVersion.canonicalStateSignature)
            .toBe(initialBody.areaStateVersion.canonicalStateSignature);

        await app.close();
      });
});
