import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {__resetSettlementPipelineForTests} from '../../domains/gameplay-session-orchestration/pipelines/settlementPipeline.ts';
import {resetGameplaySessionStore} from '../../domains/gameplay-session-orchestration/state/sessionStore.ts';
import {registerFastifyErrorMapper} from '../../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../../middleware/requestContext.ts';

import {registerV1GameplayRoutes} from './gameplay.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerV1GameplayRoutes(app);
  await app.ready();
  return app;
}

describe('v1 gameplay rejoin and terminate contracts', () => {
  beforeEach(() => {
    resetGameplaySessionStore();
    __resetSettlementPipelineForTests();
  });

  test('supports rejoin and termination paths', async () => {
    const app = await createApp();

    const admission = await app.inject({
      method: 'POST',
      url: '/v1/gameplay/sessions/admissions',
      headers: {
        'x-actor-id': 'player-2',
      },
      payload: {
        playerId: 'player-2',
        queueIntent: 'casual',
        idempotencyKey: 'admission-idem-002',
      },
    });
    const admitted = admission.json() as {
      sessionId: string;
      continuityWindowExpiresAt: string;
    };
    const continuityToken = `ct_${admitted.sessionId.split('_')[1]}deadbeef`;

    const rejoin = await app.inject({
      method: 'POST',
      url: `/v1/gameplay/sessions/${admitted.sessionId}/rejoin`,
      payload: {
        playerId: 'player-2',
        continuityToken,
      },
    });
    expect(rejoin.statusCode).toBe(200);
    expect(
        (rejoin.json() as {rejoinStatus: string}).rejoinStatus === 'expired' ||
        (rejoin.json() as {rejoinStatus: string}).rejoinStatus === 'not-found')
        .toBe(true);

    const terminate = await app.inject({
      method: 'POST',
      url: `/v1/gameplay/sessions/${admitted.sessionId}/terminate`,
      payload: {
        playerId: 'player-2',
        terminationReason: 'completed',
        idempotencyKey: 'terminate-idem-002',
      },
    });

    expect(terminate.statusCode).toBe(200);
    expect((terminate.json() as {terminationStatus: string}).terminationStatus)
        .toBe('terminated');

    await app.close();
  });
});
