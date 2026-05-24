import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetDomainChangeRecords} from '../domains/gameplay-session-orchestration/persistence/domainChangeRecorder.ts';
import {__resetSettlementPipelineForTests} from '../domains/gameplay-session-orchestration/pipelines/settlementPipeline.ts';
import {resetGameplaySessionStore} from '../domains/gameplay-session-orchestration/state/sessionStore.ts';
import {resetPlayerAccountStore} from '../domains/player-identity-account/state/accountStore.ts';
import {resetPlayerTruthStore} from '../domains/progression-inventory/state/playerTruthStore.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerV1GameplayRoutes} from '../routes/v1/gameplay.ts';
import {registerV1PlayerRoutes} from '../routes/v1/player.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerV1GameplayRoutes(app);
  await registerV1PlayerRoutes(app);
  await app.ready();
  return app;
}

describe('US2 cross-channel parity (SC-002)', () => {
  beforeEach(() => {
    resetDomainChangeRecords();
    resetGameplaySessionStore();
    resetPlayerAccountStore();
    resetPlayerTruthStore();
    __resetSettlementPipelineForTests();
  });

  test(
      'keeps account + progression + inventory aligned to one player truth',
      async () => {
        const app = await createApp();

        const admission = await app.inject({
          method: 'POST',
          url: '/v1/gameplay/sessions/admissions',
          headers: {'x-actor-id': 'player-parity-1'},
          payload: {
            playerId: 'player-parity-1',
            queueIntent: 'ranked',
            idempotencyKey: 'admission-parity-1',
          },
        });
        const {sessionId} = admission.json() as {sessionId: string};

        await app.inject({
          method: 'POST',
          url: `/v1/gameplay/sessions/${sessionId}/commands`,
          headers: {'x-actor-id': 'player-parity-1'},
          payload: {
            playerId: 'player-parity-1',
            commandType: 'reward',
            commandPayload: {
              xpDelta: 250,
              itemId: 'elixir',
              quantityDelta: 1,
            },
            commandSequence: 1,
            idempotencyKey: 'command-parity-1',
          },
        });

        const account = await app.inject({
          method: 'PATCH',
          url: '/v1/player/account',
          headers: {'x-actor-id': 'player-parity-1'},
          payload: {
            profilePatch: {
              displayName: 'ParityHero',
              region: 'NA',
            },
            expectedVersion: 0,
            idempotencyKey: 'account-parity-1',
          },
        });
        expect(account.statusCode).toBe(200);

        const accountView = await app.inject({
          method: 'GET',
          url: '/v1/player/account',
          headers: {'x-actor-id': 'player-parity-1'},
        });
        const progressionView = await app.inject({
          method: 'GET',
          url: '/v1/player/progression',
          headers: {'x-actor-id': 'player-parity-1'},
        });
        const inventoryView = await app.inject({
          method: 'GET',
          url: '/v1/player/inventory',
          headers: {'x-actor-id': 'player-parity-1'},
        });

        expect(accountView.statusCode).toBe(200);
        expect(progressionView.statusCode).toBe(200);
        expect(inventoryView.statusCode).toBe(200);

        const accountBody = accountView.json() as {
          playerId: string;
          version: number
        };
        const progressionBody = progressionView.json() as {
          playerId: string;
          progression: {xp: number};
        };
        const inventoryBody = inventoryView.json() as {
          playerId: string;
          inventory: Array<{itemId: string; quantity: number}>;
        };

        expect(accountBody.playerId).toBe('player-parity-1');
        expect(progressionBody.playerId).toBe(accountBody.playerId);
        expect(inventoryBody.playerId).toBe(accountBody.playerId);
        expect(accountBody.version).toBe(1);
        expect(progressionBody.progression.xp).toBe(250);
        expect(inventoryBody.inventory.some((item) => item.itemId === 'elixir'))
            .toBe(true);

        await app.close();
      });
});
