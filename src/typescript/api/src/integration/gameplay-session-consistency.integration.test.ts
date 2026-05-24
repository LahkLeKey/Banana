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

describe('US1 gameplay session consistency (SC-001)', () => {
  beforeEach(() => {
    resetGameplaySessionStore();
    __resetSettlementPipelineForTests();
  });

  test(
      'converges concurrent commands to one authoritative state within 2 seconds',
      async () => {
        const app = await createApp();

        const admission = await app.inject({
          method: 'POST',
          url: '/v1/gameplay/sessions/admissions',
          headers: {'x-actor-id': 'player-consistency'},
          payload: {
            playerId: 'player-consistency',
            queueIntent: 'ranked',
            idempotencyKey: 'admission-idem-consistency',
          },
        });
        const {sessionId} = admission.json() as {sessionId: string};

        const startedAt = Date.now();
        await Promise.all(
            Array.from({length: 20}).map((_, index) => app.inject({
              method: 'POST',
              url: `/v1/gameplay/sessions/${sessionId}/commands`,
              headers: {'x-actor-id': 'player-consistency'},
              payload: {
                playerId: 'player-consistency',
                commandType: 'move',
                commandPayload: {x: index, z: 0},
                commandSequence: index + 1,
                idempotencyKey: `cmd-idem-${index}`,
              },
            })));

        const elapsed = Date.now() - startedAt;
        const session = getSessionById(sessionId);
        expect(session).toBeTruthy();
        expect(session?.authoritativeTick).toBe(20);
        expect(elapsed).toBeLessThanOrEqual(2000);

        await app.close();
      });
});
