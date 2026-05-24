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

describe('US3 player insights usability (SC-003)', () => {
  beforeEach(() => {
    resetDomainChangeRecords();
    resetGameplaySessionStore();
    resetPlayerAccountStore();
    resetPlayerTruthStore();
    __resetSettlementPipelineForTests();
  });

  test(
      'returns noData=true for new players and noData=false once activity exists',
      async () => {
        const app = await createApp();

        const noData = await app.inject({
          method: 'GET',
          url: '/v1/player/insights',
          headers: {'x-actor-id': 'player-insight-new'},
        });
        expect(noData.statusCode).toBe(200);
        expect((noData.json() as {noData: boolean}).noData).toBe(true);

        const admission = await app.inject({
          method: 'POST',
          url: '/v1/gameplay/sessions/admissions',
          headers: {'x-actor-id': 'player-insight-new'},
          payload: {
            playerId: 'player-insight-new',
            queueIntent: 'casual',
            idempotencyKey: 'admission-insight-new',
          },
        });
        const {sessionId} = admission.json() as {sessionId: string};

        await app.inject({
          method: 'POST',
          url: `/v1/gameplay/sessions/${sessionId}/commands`,
          headers: {'x-actor-id': 'player-insight-new'},
          payload: {
            playerId: 'player-insight-new',
            commandType: 'reward',
            commandPayload: {
              xpDelta: 10,
              itemId: 'coin',
              quantityDelta: 3,
            },
            commandSequence: 1,
            idempotencyKey: 'command-insight-new',
          },
        });

        const withData = await app.inject({
          method: 'GET',
          url: '/v1/player/insights',
          headers: {'x-actor-id': 'player-insight-new'},
        });

        expect(withData.statusCode).toBe(200);
        const payload = withData.json() as {
          noData: boolean;
          progressionSummary: {xp: number};
          inventoryTrendSummary: {distinctItems: number};
        };
        expect(payload.noData).toBe(false);
        expect(payload.progressionSummary.xp).toBe(10);
        expect(payload.inventoryTrendSummary.distinctItems).toBe(1);

        await app.close();
      });
});
