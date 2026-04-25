import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';

import {__resetChatRouteStateForTests, registerChatRoutes} from './routes.ts';

async function createApp() {
  const app = Fastify({logger: false});
  __resetChatRouteStateForTests();
  await registerChatRoutes(app);
  await app.ready();
  return app;
}

describe('chat routes', () => {
  test(
      'creates a session, posts a message, and replays idempotent duplicate',
      async () => {
        const app = await createApp();
        const sessionRes = await app.inject({
          method: 'POST',
          url: '/chat/sessions',
          payload: {platform: 'web'},
        });

        expect(sessionRes.statusCode).toBe(201);
        const sessionPayload = sessionRes.json() as {
          session: {id: string};
          welcome_message: {role: string};
        };
        expect(sessionPayload.session.id).toStartWith('chat_');
        expect(sessionPayload.welcome_message.role).toBe('assistant');

        const messageRes = await app.inject({
          method: 'POST',
          url: `/chat/sessions/${sessionPayload.session.id}/messages`,
          payload: {
            content: 'ripe banana bunch from harvest crate',
            client_message_id: 'm1',
          },
        });

        expect(messageRes.statusCode).toBe(201);
        const messagePayload = messageRes.json() as {
          duplicate: boolean;
          user_message: {id: string; role: string};
          assistant_message: {id: string; role: string; content: string};
        };
        expect(messagePayload.duplicate).toBe(false);
        expect(messagePayload.user_message.role).toBe('user');
        expect(messagePayload.assistant_message.role).toBe('assistant');
        expect(messagePayload.assistant_message.content).toContain('banana');
        expect(messagePayload.assistant_message.content).toContain('[model=');

        const duplicateRes = await app.inject({
          method: 'POST',
          url: `/chat/sessions/${sessionPayload.session.id}/messages`,
          payload: {
            content: 'ripe banana bunch from harvest crate',
            client_message_id: 'm1',
          },
        });

        expect(duplicateRes.statusCode).toBe(200);
        const duplicatePayload = duplicateRes.json() as {
          duplicate: boolean;
          user_message: {id: string};
          assistant_message: {id: string};
        };
        expect(duplicatePayload.duplicate).toBe(true);
        expect(duplicatePayload.user_message.id)
            .toBe(messagePayload.user_message.id);
        expect(duplicatePayload.assistant_message.id)
            .toBe(messagePayload.assistant_message.id);

        await app.close();
      });

  test('returns typed error for missing session', async () => {
    const app = await createApp();
    const res = await app.inject({
      method: 'GET',
      url: '/chat/sessions/unknown-session',
    });

    expect(res.statusCode).toBe(404);
    const payload = res.json() as {error: {code: string; retryable: boolean}};
    expect(payload.error.code).toBe('session_not_found');
    expect(payload.error.retryable).toBe(false);

    await app.close();
  });
});