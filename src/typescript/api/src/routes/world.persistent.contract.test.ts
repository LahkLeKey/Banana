import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';

import {createWorldServiceForTests} from './world.test-service.ts';
import {registerWorldRoutes} from './world.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerWorldRoutes(app, {worldService: createWorldServiceForTests()});
  await app.ready();
  return app;
}

describe('world authoritative persistence contract', () => {
  test(
      'persists deterministic baseline + delta and supports replay lookup',
      async () => {
        const app = await createApp();
        const payload = {
          worldId: '550e8400-e29b-41d4-a716-446655440000',
          laneId: 'lane-primary',
          chunkX: 3,
          chunkY: 8,
          partitionEpoch: 2,
          baseAreaStateVersion: 0,
          idempotencyKey: 'idempotency-a',
          deltaPayload: {op: 'place', item: 'merchant-sign'},
        };

        const apply = await app.inject({
          method: 'POST',
          url: '/api/world/state/delta',
          payload,
        });

        expect(apply.statusCode).toBe(200);
        const applied = apply.json() as {
          areaIdentity: {areaId: string};
          areaStateVersion:
              {areaStateVersion: number; appliedDeltaThroughSequence: number};
        };
        expect(applied.areaStateVersion.areaStateVersion).toBe(1);
        expect(applied.areaStateVersion.appliedDeltaThroughSequence).toBe(1);

        const replay = await app.inject({
          method: 'GET',
          url: `/api/world/state/${applied.areaIdentity.areaId}`,
        });

        expect(replay.statusCode).toBe(200);
        const replayed = replay.json() as {
          areaStateVersion:
              {areaStateVersion: number; appliedDeltaThroughSequence: number};
        };
        expect(replayed.areaStateVersion.areaStateVersion).toBe(1);
        expect(replayed.areaStateVersion.appliedDeltaThroughSequence).toBe(1);

        await app.close();
      });

  test('duplicate idempotency key is a no-op', async () => {
    const app = await createApp();
    const payload = {
      worldId: '550e8400-e29b-41d4-a716-446655440000',
      laneId: 'lane-primary',
      chunkX: 6,
      chunkY: 1,
      partitionEpoch: 2,
      baseAreaStateVersion: 0,
      idempotencyKey: 'idempotency-repeat',
      deltaPayload: {op: 'paint', brush: 'grass'},
    };

    const first = await app.inject(
        {method: 'POST', url: '/api/world/state/delta', payload});
    const second = await app.inject(
        {method: 'POST', url: '/api/world/state/delta', payload});

    expect(first.statusCode).toBe(200);
    expect(second.statusCode).toBe(200);

    const firstBody =
        first.json() as {areaStateVersion: {areaStateVersion: number}};
    const secondBody =
        second.json() as {areaStateVersion: {areaStateVersion: number}};
    expect(secondBody.areaStateVersion.areaStateVersion)
        .toBe(firstBody.areaStateVersion.areaStateVersion);

    await app.close();
  });

  test('rejects mesh-like delta persistence payloads', async () => {
    const app = await createApp();
    const payload = {
      worldId: '550e8400-e29b-41d4-a716-446655440000',
      laneId: 'lane-primary',
      chunkX: 2,
      chunkY: 5,
      partitionEpoch: 2,
      baseAreaStateVersion: 0,
      idempotencyKey: 'idempotency-mesh-blob',
      deltaPayload: {
        op: 'replace',
        meshBlob: 'pretend-client-binary',
      },
    };

    const response = await app.inject({
      method: 'POST',
      url: '/api/world/state/delta',
      payload,
    });

    expect(response.statusCode).toBe(400);
    expect(response.json()).toMatchObject({
      error: {
        code: 'validation_error',
        message: 'persistent_world_mesh_blob_not_allowed',
      },
    });

    await app.close();
  });

  test('reports storage diagnostics under the 10 GB cap', async () => {
    const app = await createApp();

    const response = await app.inject({
      method: 'POST',
      url: '/api/world/storage/diagnostics',
      payload: {
        baselineAreaCount: 250000,
        deltaRecordCount: 180000,
        progressionLedgerEntryCount: 240000,
        averageBaselineMetadataBytes: 512,
        averageDeltaRecordBytes: 1536,
        averageProgressionLedgerBytes: 768,
      },
    });

    expect(response.statusCode).toBe(200);
    expect(response.json()).toMatchObject({
      withinBudget: true,
      largestConsumer: 'terrain_deltas',
    });

    await app.close();
  });

  test(
      'reports over-budget storage diagnostics when simulations exceed 10 GB',
      async () => {
        const app = await createApp();

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/storage/diagnostics',
          payload: {
            baselineAreaCount: 500000,
            deltaRecordCount: 3000000,
            progressionLedgerEntryCount: 4000000,
            averageBaselineMetadataBytes: 1024,
            averageDeltaRecordBytes: 3072,
            averageProgressionLedgerBytes: 1024,
          },
        });

        expect(response.statusCode).toBe(200);
        expect(response.json()).toMatchObject({
          withinBudget: false,
          remainingBytes: 0,
          largestConsumer: 'terrain_deltas',
        });

        await app.close();
      });

  test(
      'reports version drift diagnostics for synchronized native and client versions',
      async () => {
        const app = await createApp();

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/version/diagnostics',
          payload: {
            nativeGeneratorVersion: 'county-static-v1',
            clientProfileVersion: 'banana-archipelago-region-baseline-v1',
          },
        });

        expect(response.statusCode).toBe(200);
        expect(response.json()).toMatchObject({
          driftDetected: false,
          mismatches: [],
        });

        await app.close();
      });

  test(
      'reports version drift diagnostics when native and client contracts diverge',
      async () => {
        const app = await createApp();

        const response = await app.inject({
          method: 'POST',
          url: '/api/world/version/diagnostics',
          payload: {
            nativeGeneratorVersion: 'county-static-v2',
            clientProfileVersion: 'banana-archipelago-region-baseline-v2',
          },
        });

        expect(response.statusCode).toBe(200);
        expect(response.json()).toMatchObject({
          driftDetected: true,
        });

        await app.close();
      });
});
