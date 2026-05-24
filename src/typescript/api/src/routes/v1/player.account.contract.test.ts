import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetDomainChangeRecords} from '../../domains/gameplay-session-orchestration/persistence/domainChangeRecorder.ts';
import {resetPlayerAccountStore} from '../../domains/player-identity-account/state/accountStore.ts';
import {resetPlayerTruthStore} from '../../domains/progression-inventory/state/playerTruthStore.ts';
import {registerFastifyErrorMapper} from '../../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../../middleware/requestContext.ts';

import {registerV1PlayerRoutes} from './player.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerV1PlayerRoutes(app);
  await app.ready();
  return app;
}

describe('v1 player account contracts', () => {
  beforeEach(() => {
    resetDomainChangeRecords();
    resetPlayerAccountStore();
    resetPlayerTruthStore();
  });

  test('returns account and accepts versioned update', async () => {
    const app = await createApp();

    const account = await app.inject({
      method: 'GET',
      url: '/v1/player/account',
      headers: {'x-actor-id': 'player-account-1'},
    });
    expect(account.statusCode).toBe(200);
    expect((account.json() as {version: number}).version).toBe(0);

    const updated = await app.inject({
      method: 'PATCH',
      url: '/v1/player/account',
      headers: {'x-actor-id': 'player-account-1'},
      payload: {
        profilePatch: {displayName: 'Aster'},
        expectedVersion: 0,
        idempotencyKey: 'account-update-0001',
      },
    });

    expect(updated.statusCode).toBe(200);
    expect((updated.json() as {version: number}).version).toBe(1);

    await app.close();
  });

  test('returns deterministic 409 for version conflict', async () => {
    const app = await createApp();

    await app.inject({
      method: 'PATCH',
      url: '/v1/player/account',
      headers: {'x-actor-id': 'player-account-2'},
      payload: {
        profilePatch: {displayName: 'Riven'},
        expectedVersion: 0,
        idempotencyKey: 'account-update-0002',
      },
    });

    const conflict = await app.inject({
      method: 'PATCH',
      url: '/v1/player/account',
      headers: {'x-actor-id': 'player-account-2'},
      payload: {
        profilePatch: {displayName: 'Nova'},
        expectedVersion: 0,
        idempotencyKey: 'account-update-0003',
      },
    });

    expect(conflict.statusCode).toBe(409);
    expect((conflict.json() as {error: {code: string}}).error.code)
        .toBe('conflict');

    await app.close();
  });
});
