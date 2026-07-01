import {beforeEach, describe, expect, it, mock} from 'bun:test';
import Fastify from 'fastify';
import type {FastifyInstance} from 'fastify';

import {registerAuthRoutes} from './auth.ts';

process.env.BANANA_JWT_SECRET = 'test-secret-banana-jwt-1234';
process.env.BANANA_KEYCLOAK_ISSUER_URL =
  'http://localhost:8080/realms/banana';
process.env.BANANA_KEYCLOAK_CLIENT_ID = 'banana-web';

function extractTokenFromLocation(location: string): string {
  const parsed = new URL(location);
  const hash = parsed.hash.startsWith('#') ? parsed.hash.slice(1) : parsed.hash;
  const params = new URLSearchParams(hash);
  const token = params.get('auth_token') ?? '';
  if (!token) {
    throw new Error('expected auth_token in redirect hash');
  }
  return token;
}

async function issueKeycloakToken(app: FastifyInstance): Promise<string> {
  const response = await app.inject({
    method: 'GET',
    url:
        '/auth/keycloak/callback?code=auth-code-1&sub=keycloak:user-123&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
    headers: {
      host: 'api.banana.engineer',
    },
  });

  return extractTokenFromLocation(response.headers.location ?? '');
}

describe('auth routes', () => {
  beforeEach(() => {
    mock.restore();
  });

  it('redirects Keycloak start requests to the OIDC endpoint', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url: '/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(302);
    expect(response.headers.location ?? '')
        .toContain(
            'http://localhost:8080/realms/banana/protocol/openid-connect/auth');
    expect(response.headers.location ?? '')
        .toContain('response_type=code');
    expect(response.headers.location ?? '')
        .toContain(
            'redirect_uri=http%3A%2F%2Fapi.banana.engineer%2Fauth%2Fkeycloak%2Fcallback%3FreturnTo%3Dhttps%253A%252F%252Fbanana.engineer%252Flogin');

    await app.close();
  });

  it('does not expose legacy Steam auth routes', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const start = await app.inject({
      method: 'GET',
      url: '/auth/steam/start',
    });
    const callback = await app.inject({
      method: 'GET',
      url: '/auth/steam/callback',
    });

    expect(start.statusCode).toBe(404);
    expect(callback.statusCode).toBe(404);

    await app.close();
  });

  it('redirects Keycloak callbacks back to the login return url with a token hash',
     async () => {
       const app = Fastify();
       await registerAuthRoutes(app);

       const response = await app.inject({
         method: 'GET',
         url:
             '/auth/keycloak/callback?code=auth-code-2&sub=keycloak:user-xyz&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
         headers: {
           host: 'api.banana.engineer',
         },
       });

       expect(response.statusCode).toBe(302);
       expect(response.headers.location ?? '')
           .toContain('https://banana.engineer/login#');
       expect(response.headers.location ?? '').toContain('auth_token=');
       expect(response.headers.location ?? '').toContain('subject=keycloak%3Auser-xyz');

       await app.close();
     });

  it('validates active auth sessions from bearer token', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);
    const token = await issueKeycloakToken(app);

    const response = await app.inject({
      method: 'GET',
      url: '/auth/session',
      headers: {
        authorization: `Bearer ${token}`,
      },
    });

    expect(response.statusCode).toBe(200);
    const body = response.json() as {
      authenticated: boolean;
      identityId: string;
    };
    expect(body.authenticated).toBe(true);
    expect(body.identityId).toBe('keycloak:user-123');
    expect(body.identityKind).toBe('keycloak');

    await app.close();
  });

  it('revokes a session and rejects it after logout', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);
    const token = await issueKeycloakToken(app);

    const logout = await app.inject({
      method: 'POST',
      url: '/auth/logout',
      headers: {
        authorization: `Bearer ${token}`,
      },
    });
    expect(logout.statusCode).toBe(200);
    expect((logout.json() as {revoked: boolean}).revoked).toBe(true);

    const session = await app.inject({
      method: 'GET',
      url: '/auth/session',
      headers: {
        authorization: `Bearer ${token}`,
      },
    });
    expect(session.statusCode).toBe(401);

    await app.close();
  });

  it('creates guest auth sessions without Steam login', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const start = await app.inject({
      method: 'POST',
      url: '/auth/guest/start',
      payload: {alias: 'Notebook Ranger'},
    });

    expect(start.statusCode).toBe(200);
    const startBody = start.json() as {
      token: string;
      guestId: string;
      displayName: string;
    };
    expect(startBody.token.length).toBeGreaterThan(20);
    expect(startBody.guestId.length).toBeGreaterThan(5);
    expect(startBody.displayName).toBe('Notebook Ranger');

    const session = await app.inject({
      method: 'GET',
      url: '/auth/session',
      headers: {
        authorization: `Bearer ${startBody.token}`,
      },
    });

    expect(session.statusCode).toBe(200);
    const sessionBody = session.json() as {
      authenticated: boolean;
      identityKind: string;
      guestId: string;
    };
    expect(sessionBody.authenticated).toBe(true);
    expect(sessionBody.identityKind).toBe('guest');
    expect(sessionBody.guestId).toBe(startBody.guestId);

    await app.close();
  });
});
