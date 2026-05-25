import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetLaunchGateVerificationServiceForTests} from '../domains/player-identity-account/services/launchGateVerificationService.ts';
import {registerFastifyErrorMapper} from '../lib/errors/fastifyErrorMapper.ts';
import {registerRequestContextMiddleware} from '../middleware/requestContext.ts';
import {registerV1PlayerRoutes} from '../routes/v1/player.ts';

async function createApp() {
  const app = Fastify({logger: false});
  await registerRequestContextMiddleware(app);
  registerFastifyErrorMapper(app);
  await registerV1PlayerRoutes(app);
  await app.ready();
  return app;
}

describe('US2 launch-gate remediation integration', () => {
  beforeEach(() => {
    resetLaunchGateVerificationServiceForTests();
  });

  test(
      'denied launch can remediate and pass fresh verification on retry',
      async () => {
        const app = await createApp();

        const denied = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-remedy-flow'},
          payload: {
            mode: 'production-steam-package',
            clientBuildChannel: 'steam-prod',
          },
        });

        expect(denied.statusCode).toBe(200);
        const deniedBody = denied.json() as {
          attemptId: string;
          decision: string;
          reasonCode: string;
          remediation?: {remediationTicketId: string};
        };

        expect(deniedBody.decision).toBe('deny');
        expect(deniedBody.reasonCode).toBe('STEAM_UNAVAILABLE');
        expect(deniedBody.remediation?.remediationTicketId?.length ?? 0)
            .toBeGreaterThan(0);

        const complete = await app.inject({
          method: 'POST',
          url: `/v1/player/account-link-remediation/ticket/${
              deniedBody.remediation?.remediationTicketId}/complete`,
          headers: {'x-actor-id': 'player-remedy-flow'},
        });
        expect(complete.statusCode).toBe(200);
        expect((complete.json() as {status: string}).status).toBe('completed');

        const retryIntent = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/retry-intent',
          headers: {'x-actor-id': 'player-remedy-flow'},
          payload: {
            attemptId: deniedBody.attemptId,
            priorReasonCode: deniedBody.reasonCode,
            clientObservedAt: new Date().toISOString(),
          },
        });

        expect(retryIntent.statusCode).toBe(200);
        const retryBody = retryIntent.json() as {
          retryAccepted: boolean;
          nextAttemptId: string;
          requiresFreshVerification: boolean;
        };
        expect(retryBody.retryAccepted).toBe(true);
        expect(retryBody.requiresFreshVerification).toBe(true);
        expect(retryBody.nextAttemptId.length).toBeGreaterThan(0);

        const allowed = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-remedy-flow'},
          payload: {
            attemptId: retryBody.nextAttemptId,
            mode: 'production-steam-package',
            steamAssertion: {
              steamId: '76561198000000000',
              validationStatus: 'valid',
            },
            clientBuildChannel: 'steam-prod',
          },
        });

        expect(allowed.statusCode).toBe(200);
        expect((allowed.json() as {
                 decision: string;
                 reasonCode: string
               }).decision)
            .toBe('allow');
        expect((allowed.json() as {
                 decision: string;
                 reasonCode: string
               }).reasonCode)
            .toBe('OK');

        await app.close();
      });

  test('retry intent rejects mismatched prior reason code', async () => {
    const app = await createApp();

    const denied = await app.inject({
      method: 'POST',
      url: '/v1/player/launch-gate/verify',
      headers: {'x-actor-id': 'player-remedy-mismatch'},
      payload: {
        mode: 'production-steam-package',
        clientBuildChannel: 'steam-prod',
      },
    });

    const deniedBody = denied.json() as {attemptId: string};
    const retryIntent = await app.inject({
      method: 'POST',
      url: '/v1/player/launch-gate/retry-intent',
      headers: {'x-actor-id': 'player-remedy-mismatch'},
      payload: {
        attemptId: deniedBody.attemptId,
        priorReasonCode: 'STALE_SESSION',
        clientObservedAt: new Date().toISOString(),
      },
    });

    expect(retryIntent.statusCode).toBe(200);
    expect((retryIntent.json() as {retryAccepted: boolean}).retryAccepted)
        .toBe(false);

    await app.close();
  });
});
