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

describe('v1 account-link remediation contracts', () => {
  beforeEach(() => {
    resetLaunchGateVerificationServiceForTests();
  });

  test('creates, reads, and completes remediation tickets', async () => {
    const app = await createApp();

    const create = await app.inject({
      method: 'POST',
      url: '/v1/player/account-link-remediation/ticket',
      headers: {'x-actor-id': 'player-remediation-1'},
      payload: {
        reasonCode: 'UNLINKED_ACCOUNT',
      },
    });

    expect(create.statusCode).toBe(201);
    const created = create.json() as {
      remediationTicketId: string;
      status: string;
      requiredActions: string[];
    };

    expect(created.remediationTicketId.length).toBeGreaterThan(0);
    expect(created.status).toBe('pending');
    expect(created.requiredActions.length).toBeGreaterThan(0);

    const getTicket = await app.inject({
      method: 'GET',
      url: `/v1/player/account-link-remediation/ticket/${
          created.remediationTicketId}`,
      headers: {'x-actor-id': 'player-remediation-1'},
    });
    expect(getTicket.statusCode).toBe(200);

    const complete = await app.inject({
      method: 'POST',
      url: `/v1/player/account-link-remediation/ticket/${
          created.remediationTicketId}/complete`,
      headers: {'x-actor-id': 'player-remediation-1'},
    });

    expect(complete.statusCode).toBe(200);
    expect((complete.json() as {status: string}).status).toBe('completed');

    await app.close();
  });

  test('rejects unknown reason codes', async () => {
    const app = await createApp();

    const create = await app.inject({
      method: 'POST',
      url: '/v1/player/account-link-remediation/ticket',
      headers: {'x-actor-id': 'player-remediation-2'},
      payload: {
        reasonCode: 'NOT_A_REASON',
      },
    });

    expect(create.statusCode).toBe(400);
    expect((create.json() as {error: {code: string}}).error.code)
        .toBe('validation_error');

    await app.close();
  });
});
