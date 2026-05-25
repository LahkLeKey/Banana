import {beforeEach, describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {resetLaunchGateVerificationServiceForTests} from '../../domains/player-identity-account/services/launchGateVerificationService.ts';
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

describe('v1 launch gate contracts', () => {
  beforeEach(() => {
    resetLaunchGateVerificationServiceForTests();
  });

  test(
      'returns allow for development mode without steam assertion',
      async () => {
        const app = await createApp();

        const verify = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-launch-1'},
          payload: {
            mode: 'development',
            clientBuildChannel: 'local-dev',
          },
        });

        expect(verify.statusCode).toBe(200);
        const body = verify.json() as {
          attemptId: string;
          decision: 'allow'|'deny';
          reasonCode: string;
          effectiveMode: string;
          isRemediable: boolean;
          auditRef: string;
        };

        expect(body.decision).toBe('allow');
        expect(body.reasonCode).toBe('OK');
        expect(body.effectiveMode).toBe('development');
        expect(body.isRemediable).toBe(false);
        expect(body.attemptId.length).toBeGreaterThan(0);
        expect(body.auditRef.length).toBeGreaterThan(0);

        const status = await app.inject({
          method: 'GET',
          url: `/v1/player/launch-gate/status/${body.attemptId}`,
          headers: {'x-actor-id': 'player-launch-1'},
        });

        expect(status.statusCode).toBe(200);
        expect((status.json() as {decision: string}).decision).toBe('allow');

        await app.close();
      });

  test(
      'returns deny taxonomy in production for missing and expired assertions',
      async () => {
        const app = await createApp();

        const missingAssertion = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-launch-2'},
          payload: {
            mode: 'production-steam-package',
            clientBuildChannel: 'steam-prod',
          },
        });

        expect(missingAssertion.statusCode).toBe(200);
        const missingBody = missingAssertion.json() as {
          decision: string;
          reasonCode: string;
          isRemediable: boolean;
        };
        expect(missingBody.decision).toBe('deny');
        expect(missingBody.reasonCode).toBe('STEAM_UNAVAILABLE');
        expect(missingBody.isRemediable).toBe(true);

        const expiredAssertion = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-launch-2'},
          payload: {
            mode: 'production-steam-package',
            steamAssertion: {
              steamId: '76561198000000000',
              validationStatus: 'expired',
            },
            clientBuildChannel: 'steam-prod',
          },
        });

        expect(expiredAssertion.statusCode).toBe(200);
        const expiredBody = expiredAssertion.json() as {
          decision: string;
          reasonCode: string;
          isRemediable: boolean;
          remediation?: {remediationTicketId: string};
        };
        expect(expiredBody.decision).toBe('deny');
        expect(expiredBody.reasonCode).toBe('STALE_SESSION');
        expect(expiredBody.isRemediable).toBe(true);
        expect(expiredBody.remediation?.remediationTicketId?.length ?? 0)
            .toBeGreaterThan(0);

        await app.close();
      });

  test(
      'returns deny with UNKNOWN_MODE for unsupported mode strings',
      async () => {
        const app = await createApp();

        const verify = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-launch-3'},
          payload: {
            mode: 'arcade-kiosk',
            clientBuildChannel: 'custom',
          },
        });

        expect(verify.statusCode).toBe(200);
        const body = verify.json() as {
          decision: string;
          reasonCode: string;
          effectiveMode: string;
          isRemediable: boolean;
        };

        expect(body.decision).toBe('deny');
        expect(body.reasonCode).toBe('UNKNOWN_MODE');
        expect(body.effectiveMode).toBe('production-steam-package');
        expect(body.isRemediable).toBe(false);

        await app.close();
      });
});
