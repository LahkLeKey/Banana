import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetDomainChangeRecords} from '../../domains/gameplay-session-orchestration/persistence/domainChangeRecorder.ts';
import {resetGameplaySessionStore} from '../../domains/gameplay-session-orchestration/state/sessionStore.ts';
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

describe('v1 player insights contract', () => {
  beforeEach(() => {
    resetDomainChangeRecords();
    resetGameplaySessionStore();
    resetPlayerAccountStore();
    resetPlayerTruthStore();
  });

  test(
      'returns typed insights payload including noData and freshness metadata',
      async () => {
        const app = await createApp();

        const response = await app.inject({
          method: 'GET',
          url: '/v1/player/insights',
          headers: {'x-actor-id': 'player-insights-1'},
        });

        expect(response.statusCode).toBe(200);
        const payload = response.json() as {
          playerId: string;
          noData: boolean;
          freshnessTimestamp: string;
          sessionSummary: Record<string, unknown>;
          progressionSummary: Record<string, unknown>;
          inventoryTrendSummary: Record<string, unknown>;
        };
        expect(payload.playerId).toBe('player-insights-1');
        expect(typeof payload.noData).toBe('boolean');
        expect(payload.freshnessTimestamp.length).toBeGreaterThan(0);
        expect(typeof payload.sessionSummary).toBe('object');
        expect(typeof payload.progressionSummary).toBe('object');
        expect(typeof payload.inventoryTrendSummary).toBe('object');

        await app.close();
      });
});
