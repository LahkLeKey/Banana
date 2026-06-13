import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerWorldRoutes} from '../routes/world.ts';
import {createWorldServiceForTests} from '../routes/world.test-service.ts';

type ReplayResponse = {
  areaIdentity: {
    worldId: string; laneId: string; chunkX: number; chunkY: number;
    partitionEpoch: number;
    areaId: string;
    identityHash: string;
  };
  baselineId: string;
  areaStateVersion: {
    areaId: string; areaStateVersion: number;
    appliedDeltaThroughSequence: number;
    canonicalStateSignature: string;
  };
  orchestrationPathId: string;
  chunk: {chunkX: number; chunkY: number; sizeBytes: number;};
};

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app, {worldService: createWorldServiceForTests()});
  await app.ready();
  return app;
}

describe('US1 persistent-world revisit parity integration', () => {
  test(
      'revisit replay returns deterministic baseline across sessions',
      async () => {
        const payload = {
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'primary',
          chunkX: 4,
          chunkY: -2,
          partitionEpoch: 1,
        };

        const firstSessionApp = await createApp();
        const firstReplay = await firstSessionApp.inject({
          method: 'POST',
          url: '/api/world/chunk/replay',
          payload,
        });
        expect(firstReplay.statusCode).toBe(200);
        const firstBody = firstReplay.json() as ReplayResponse;

        await firstSessionApp.close();

        const secondSessionApp = await createApp();
        const secondReplay = await secondSessionApp.inject({
          method: 'POST',
          url: '/api/world/chunk/replay',
          payload,
        });
        expect(secondReplay.statusCode).toBe(200);
        const secondBody = secondReplay.json() as ReplayResponse;

        expect(secondBody.areaIdentity.identityHash)
            .toBe(firstBody.areaIdentity.identityHash);
        expect(secondBody.areaIdentity.areaId)
            .toBe(firstBody.areaIdentity.areaId);
        expect(secondBody.baselineId).toBe(firstBody.baselineId);
        expect(secondBody.orchestrationPathId)
            .toBe(firstBody.orchestrationPathId);
        expect(secondBody.areaStateVersion.canonicalStateSignature)
            .toBe(firstBody.areaStateVersion.canonicalStateSignature);
        expect(secondBody.chunk.sizeBytes).toBe(firstBody.chunk.sizeBytes);

        expect(secondBody.areaStateVersion.areaStateVersion).toBe(0);
        expect(secondBody.chunk.sizeBytes).toBeGreaterThan(0);

        await secondSessionApp.close();
      });
});
