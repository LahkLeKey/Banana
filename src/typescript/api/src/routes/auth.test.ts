import {beforeEach, describe, expect, it, mock} from 'bun:test';
import Fastify from 'fastify';
import type {FastifyInstance} from 'fastify';

import {registerAuthRoutes} from './auth.ts';

process.env.BANANA_JWT_SECRET = 'test-secret-banana-jwt-1234';
process.env.BANANA_KEYCLOAK_ISSUER_URL = 'http://localhost:8080/realms/banana';
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

function makeUnsignedJwt(payload: Record<string, unknown>): string {
  const encoded =
      Buffer.from(JSON.stringify(payload), 'utf8').toString('base64url');
  return `header.${encoded}.signature`;
}

function makeAuthState(codeVerifier = 'test-code-verifier'): string {
  return Buffer
      .from(JSON.stringify({nonce: 'test-nonce', codeVerifier}), 'utf8')
      .toString('base64url');
}

function mockTokenExchange(
    subject = 'user-123', expectedVerifier = 'test-code-verifier') {
  const idToken = makeUnsignedJwt({sub: subject});
  globalThis.fetch =
      mock(async (_input, init) => {
        const body = typeof init?.body === 'string' ?
            new URLSearchParams(init.body) :
            new URLSearchParams(String(init?.body ?? ''));
        expect(body.get('code_verifier') ?? '').toBe(expectedVerifier);
        return new Response(JSON.stringify({id_token: idToken}), {
          status: 200,
          headers: {'content-type': 'application/json'},
        });
      }) as unknown as typeof fetch;
}

async function issueKeycloakToken(app: FastifyInstance): Promise<string> {
  mockTokenExchange('user-123');
  const state = makeAuthState();

  const response = await app.inject({
    method: 'GET',
    url: `/auth/keycloak/callback?code=auth-code-1&state=${
        state}&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin`,
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
      url:
          '/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(302);
    expect(response.headers.location ?? '')
        .toContain(
            'http://localhost:8080/realms/banana/protocol/openid-connect/auth');
    expect(response.headers.location ?? '').toContain('response_type=code');
    expect(response.headers.location ?? '')
        .toContain('code_challenge_method=S256');
    expect(response.headers.location ?? '').toContain('code_challenge=');
    expect(response.headers.location ?? '')
        .toContain(
            'redirect_uri=http%3A%2F%2Fapi.banana.engineer%2Fauth%2Fkeycloak%2Fcallback%3FreturnTo%3Dhttps%253A%252F%252Fbanana.engineer%252Flogin');

    await app.close();
  });

  it('passes provider hint to Keycloak start redirect', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url:
          '/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin&provider=google',
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(302);
    expect(response.headers.location ?? '').toContain('kc_idp_hint=google');

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
       mockTokenExchange('user-xyz');
       const state = makeAuthState();

       const response = await app.inject({
         method: 'GET',
         url: `/auth/keycloak/callback?code=auth-code-2&state=${
             state}&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin`,
         headers: {
           host: 'api.banana.engineer',
         },
       });

       expect(response.statusCode).toBe(302);
       expect(response.headers.location ?? '')
           .toContain('https://banana.engineer/login#');
       expect(response.headers.location ?? '').toContain('auth_token=');
       expect(response.headers.location ?? '')
           .toContain('subject=keycloak%3Auser-xyz');

       await app.close();
     });

  it('rejects callbacks without authorization code', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url:
          '/auth/keycloak/callback?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(400);
    expect(response.json()).toEqual({error: 'keycloak_auth_not_completed'});

    await app.close();
  });

  it('rejects callbacks when auth state is missing', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url:
          '/auth/keycloak/callback?code=auth-code-3&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(400);
    expect(response.json()).toEqual({error: 'keycloak_auth_state_missing'});

    await app.close();
  });

  it('rejects callbacks when token exchange fails', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    globalThis.fetch = mock(async () => {
                         return new Response('invalid_grant', {status: 400});
                       }) as unknown as typeof fetch;

    const response = await app.inject({
      method: 'GET',
      url: `/auth/keycloak/callback?code=bad-code&state=${
          makeAuthState()}&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin`,
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(401);
    expect(response.json()).toEqual({
      error: 'keycloak_token_exchange_failed',
      status: 400,
      detail: 'invalid_grant',
    });

    await app.close();
  });

  it('uses access token subject when id token is missing', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    globalThis.fetch =
        mock(async () => {
          return new Response(
              JSON.stringify({access_token: makeUnsignedJwt({sub: 'user-access'})}), {
                status: 200,
                headers: {'content-type': 'application/json'},
              });
        }) as unknown as typeof fetch;

    const response = await app.inject({
      method: 'GET',
      url: `/auth/keycloak/callback?code=auth-code-4&state=${
          makeAuthState()}&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin`,
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(302);
    expect(response.headers.location ?? '')
        .toContain('subject=keycloak%3Auser-access');

    await app.close();
  });

  it('rejects callbacks when identity subject is not available', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    globalThis.fetch =
        mock(async () => {
          return new Response(JSON.stringify({id_token: makeUnsignedJwt({})}), {
            status: 200,
            headers: {'content-type': 'application/json'},
          });
        }) as unknown as typeof fetch;

    const response = await app.inject({
      method: 'GET',
      url: `/auth/keycloak/callback?code=auth-code-5&state=${
          makeAuthState()}&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin`,
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(401);
    expect(response.json()).toEqual({error: 'keycloak_identity_not_available'});

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

  it('rejects /auth/session when bearer token is missing', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url: '/auth/session',
    });

    expect(response.statusCode).toBe(401);
    expect(response.json()).toEqual({error: 'missing_bearer_token'});

    await app.close();
  });

  it('rejects /auth/session when bearer token is invalid', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url: '/auth/session',
      headers: {
        authorization: 'Bearer not-a-valid-token',
      },
    });

    expect(response.statusCode).toBe(401);
    expect(response.json()).toEqual({error: 'invalid_or_expired_token'});

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

  it('rejects /auth/logout when bearer token is missing', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'POST',
      url: '/auth/logout',
    });

    expect(response.statusCode).toBe(401);
    expect(response.json()).toEqual({error: 'missing_bearer_token'});

    await app.close();
  });

  it('rejects /auth/logout when bearer token is invalid', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'POST',
      url: '/auth/logout',
      headers: {
        authorization: 'Bearer not-a-valid-token',
      },
    });

    expect(response.statusCode).toBe(401);
    expect(response.json()).toEqual({error: 'invalid_or_expired_token'});

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

  it('normalizes empty or unsafe guest aliases to Guest', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const start = await app.inject({
      method: 'POST',
      url: '/auth/guest/start',
      payload: {alias: '!!!'},
    });

    expect(start.statusCode).toBe(200);
    expect((start.json() as {displayName: string}).displayName).toBe('Guest');

    await app.close();
  });
});
