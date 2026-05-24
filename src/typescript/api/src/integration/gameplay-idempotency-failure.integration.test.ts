import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {__resetSettlementPipelineForTests} from '../domains/gameplay-session-orchestration/pipelines/settlementPipeline.ts';
import {getSessionById, resetGameplaySessionStore} from '../domains/gameplay-session-orchestration/state/sessionStore.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerV1GameplayRoutes} from '../routes/v1/gameplay.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerV1GameplayRoutes(app);
  await app.ready();
  return app;
}

describe('US1 idempotency failure injection (SC-004)', () => {
  beforeEach(() => {
    resetGameplaySessionStore();
    __resetSettlementPipelineForTests();
  });

  test('deduplicates duplicate mutation keys', async () => {
    const app = await createApp();

    const admission = await app.inject({
      method: 'POST',
      url: '/v1/gameplay/sessions/admissions',
      headers: {'x-actor-id': 'player-idem'},
      payload: {
        playerId: 'player-idem',
        queueIntent: 'ranked',
        idempotencyKey: 'admission-idem',
      },
    });
    const {sessionId} = admission.json() as {sessionId: string};

    const duplicateKey = 'cmd-idem-duplicate';
    await Promise.all([
      app.inject({
        method: 'POST',
        url: `/v1/gameplay/sessions/${sessionId}/commands`,
        headers: {'x-actor-id': 'player-idem'},
        payload: {
          playerId: 'player-idem',
          commandType: 'attack',
          commandPayload: {ability: 'primary'},
          commandSequence: 1,
          idempotencyKey: duplicateKey,
        },
      }),
      app.inject({
        method: 'POST',
        url: `/v1/gameplay/sessions/${sessionId}/commands`,
        headers: {'x-actor-id': 'player-idem'},
        payload: {
          playerId: 'player-idem',
          commandType: 'attack',
          commandPayload: {ability: 'primary'},
          commandSequence: 1,
          idempotencyKey: duplicateKey,
        },
      }),
    ]);

    const session = getSessionById(sessionId);
    expect(session?.authoritativeTick).toBe(1);
    expect(session?.commands.length).toBe(1);

    await app.close();
  });
});
