import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetDomainChangeRecords} from '../../domains/gameplay-session-orchestration/persistence/domainChangeRecorder.ts';
import {resetGameplaySessionStore} from '../../domains/gameplay-session-orchestration/state/sessionStore.ts';
import {resetPlayerAccountStore} from '../../domains/player-identity-account/state/accountStore.ts';
import {resetPlayerTruthStore} from '../../domains/progression-inventory/state/playerTruthStore.ts';
import {registerFastifyErrorMapper} from '../../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../../middleware/requestContext.ts';

import {registerV1GameplayRoutes} from './gameplay.ts';
import {registerV1PlayerRoutes} from './player.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerV1GameplayRoutes(app);
  await registerV1PlayerRoutes(app);
  await app.ready();
  return app;
}

describe('v1 player progression and inventory contracts', () => {
  beforeEach(() => {
    resetDomainChangeRecords();
    resetGameplaySessionStore();
    resetPlayerAccountStore();
    resetPlayerTruthStore();
  });

  test(
      'exposes progression and inventory snapshots with freshness metadata',
      async () => {
        const app = await createApp();

        const admission = await app.inject({
          method: 'POST',
          url: '/v1/gameplay/sessions/admissions',
          headers: {'x-actor-id': 'player-pi-1'},
          payload: {
            playerId: 'player-pi-1',
            queueIntent: 'casual',
            idempotencyKey: 'admission-player-pi-1',
          },
        });
        const {sessionId} = admission.json() as {sessionId: string};

        await app.inject({
          method: 'POST',
          url: `/v1/gameplay/sessions/${sessionId}/commands`,
          headers: {'x-actor-id': 'player-pi-1'},
          payload: {
            playerId: 'player-pi-1',
            commandType: 'grant',
            commandPayload: {
              xpDelta: 125,
              itemId: 'potion',
              quantityDelta: 2,
            },
            commandSequence: 1,
            idempotencyKey: 'grant-player-pi-1',
          },
        });

        const progression = await app.inject({
          method: 'GET',
          url: '/v1/player/progression',
          headers: {'x-actor-id': 'player-pi-1'},
        });
        expect(progression.statusCode).toBe(200);
        const progressionBody = progression.json() as {
          progression: {xp: number};
          freshnessTimestamp: string;
          snapshotRef: string;
        };
        expect(progressionBody.progression.xp).toBe(125);
        expect(progressionBody.freshnessTimestamp.length).toBeGreaterThan(0);
        expect(progressionBody.snapshotRef.length).toBeGreaterThan(0);

        const inventory = await app.inject({
          method: 'GET',
          url: '/v1/player/inventory',
          headers: {'x-actor-id': 'player-pi-1'},
        });
        expect(inventory.statusCode).toBe(200);
        const inventoryBody = inventory.json() as {
          inventory: Array<{itemId: string; quantity: number}>;
          freshnessTimestamp: string;
          snapshotRef: string;
        };
        expect(inventoryBody.inventory[0]?.itemId).toBe('potion');
        expect(inventoryBody.inventory[0]?.quantity).toBe(2);
        expect(inventoryBody.freshnessTimestamp.length).toBeGreaterThan(0);
        expect(inventoryBody.snapshotRef.length).toBeGreaterThan(0);

        await app.close();
      });
});
