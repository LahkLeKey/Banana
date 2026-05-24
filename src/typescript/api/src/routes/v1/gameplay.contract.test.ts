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

describe('v1 gameplay admissions and commands contracts', () => {
  beforeEach(() => {
    resetGameplaySessionStore();
    __resetSettlementPipelineForTests();
  });

  test('admits player and settles command under /v1/gameplay', async () => {
    const app = await createApp();

    const admission = await app.inject({
      method: 'POST',
      url: '/v1/gameplay/sessions/admissions',
      headers: {
        'x-actor-id': 'player-1',
      },
      payload: {
        playerId: 'player-1',
        queueIntent: 'ranked',
        idempotencyKey: 'admission-idem-001',
      },
    });
    expect(admission.statusCode).toBe(201);
    const admitted = admission.json() as {sessionId: string};

    const command = await app.inject({
      method: 'POST',
      url: `/v1/gameplay/sessions/${admitted.sessionId}/commands`,
      headers: {
        'x-actor-id': 'player-1',
      },
      payload: {
        playerId: 'player-1',
        commandType: 'move',
        commandPayload: {x: 1, z: 0},
        commandSequence: 1,
        idempotencyKey: 'command-idem-001',
      },
    });

    expect(command.statusCode).toBe(200);
    expect((command.json() as {authoritativeTick: number}).authoritativeTick)
        .toBe(1);

    await app.close();
  });
});
