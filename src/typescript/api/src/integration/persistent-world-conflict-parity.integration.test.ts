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

type StateResponse = {
  areaStateVersion: {canonicalStateSignature: string; areaStateVersion: number};
};

async function runConflictScenario(laneId: string): Promise<StateResponse> {
  const app = await createApp();
  const shared = {
    worldId: '550e8400-e29b-41d4-a716-446655440000',
    laneId,
    chunkX: 2,
    chunkY: 14,
    partitionEpoch: 5,
  };

  const first = await app.inject({
    method: 'POST',
    url: '/api/world/state/delta',
    payload: {
      ...shared,
      baseAreaStateVersion: 0,
      idempotencyKey: `${laneId}-k1`,
      deltaPayload: {op: 'paint', layer: 'soil', value: 2},
    },
  });
  expect(first.statusCode).toBe(200);

  const second = await app.inject({
    method: 'POST',
    url: '/api/world/state/delta',
    payload: {
      ...shared,
      baseAreaStateVersion: 1,
      idempotencyKey: `${laneId}-k2`,
      deltaPayload: {op: 'paint', layer: 'soil', value: 3},
    },
  });
  expect(second.statusCode).toBe(200);

  const result = second.json() as StateResponse;
  await app.close();
  return result;
}

describe('US2 overlapping-delta conflict parity integration', () => {
  test(
      'equivalent overlapping delta sets produce identical canonical signatures',
      async () => {
        const runA = await runConflictScenario('lane-conflict-equivalent');
        const runB = await runConflictScenario('lane-conflict-equivalent');

        expect(runA.areaStateVersion.areaStateVersion)
            .toBe(runB.areaStateVersion.areaStateVersion);
        expect(runA.areaStateVersion.canonicalStateSignature)
            .toBe(runB.areaStateVersion.canonicalStateSignature);
      });
});
