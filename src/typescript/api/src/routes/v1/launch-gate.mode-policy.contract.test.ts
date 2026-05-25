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

describe('v1 launch gate mode-policy contracts', () => {
  beforeEach(() => {
    resetLaunchGateVerificationServiceForTests();
  });

  test(
      'ignores production override context and still denies without valid steam assertion',
      async () => {
        const app = await createApp();

        const verify = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-policy-1'},
          payload: {
            mode: 'production-steam-package',
            clientBuildChannel: 'steam-prod',
            overrideContext: {
              allowWithoutSteam: true,
              source: 'client-override',
            },
          },
        });

        expect(verify.statusCode).toBe(200);
        const body = verify.json() as {
          decision: string;
          reasonCode: string;
          effectiveMode: string;
          overrideApplied: boolean;
          overrideSource: string|null;
        };

        expect(body.decision).toBe('deny');
        expect(body.reasonCode).toBe('STEAM_UNAVAILABLE');
        expect(body.effectiveMode).toBe('production-steam-package');
        expect(body.overrideApplied).toBe(false);
        expect(body.overrideSource).toBe('client-override');

        await app.close();
      });

  test(
      'applies development mode policy without requiring steam assertion',
      async () => {
        const app = await createApp();

        const verify = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-policy-2'},
          payload: {
            mode: 'development',
            clientBuildChannel: 'local-dev',
            overrideContext: {
              source: 'local-config',
              allowNonSteamStartup: true,
            },
          },
        });

        expect(verify.statusCode).toBe(200);
        const body = verify.json() as {
          decision: string;
          reasonCode: string;
          effectiveMode: string;
          overrideApplied: boolean;
          overrideSource: string|null;
        };

        expect(body.decision).toBe('allow');
        expect(body.reasonCode).toBe('OK');
        expect(body.effectiveMode).toBe('development');
        expect(body.overrideApplied).toBe(true);
        expect(body.overrideSource).toBe('local-config');

        await app.close();
      });

  test(
      'trusted build channel metadata takes precedence over conflicting mode',
      async () => {
        const app = await createApp();

        const verify = await app.inject({
          method: 'POST',
          url: '/v1/player/launch-gate/verify',
          headers: {'x-actor-id': 'player-policy-3'},
          payload: {
            mode: 'development',
            clientBuildChannel: 'steam-prod',
            overrideContext: {
              source: 'local-config',
              allowNonSteamStartup: true,
            },
          },
        });

        expect(verify.statusCode).toBe(200);
        const body = verify.json() as {
          decision: string;
          reasonCode: string;
          effectiveMode: string;
          overrideApplied: boolean;
        };

        expect(body.effectiveMode).toBe('production-steam-package');
        expect(body.decision).toBe('deny');
        expect(body.reasonCode).toBe('STEAM_UNAVAILABLE');
        expect(body.overrideApplied).toBe(false);

        await app.close();
      });
});
