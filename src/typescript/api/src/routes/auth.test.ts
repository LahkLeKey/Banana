import {beforeEach, describe, expect, it, mock} from 'bun:test';
import Fastify from 'fastify';
import type {FastifyInstance} from 'fastify';

import {authRouteInternals, registerAuthRoutes} from './auth.ts';

process.env.BANANA_JWT_SECRET = 'test-secret-banana-jwt-1234';
process.env.BANANA_KEYCLOAK_ISSUER_URL =
    'https://banana-keycloak-dev.fly.dev/realms/banana';
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
            'https://banana-keycloak-dev.fly.dev/realms/banana/protocol/openid-connect/auth');
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

  it('passes register action to Keycloak start redirect', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url:
          '/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin&action=register',
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(302);
    expect(response.headers.location ?? '').toContain('kc_action=register');

    await app.close();
  });

  it('passes reset-password action to Keycloak start redirect', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url:
          '/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin&action=reset-password',
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(302);
    expect(response.headers.location ?? '')
        .toContain('kc_action=UPDATE_PASSWORD');

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
              JSON.stringify(
                  {access_token: makeUnsignedJwt({sub: 'user-access'})}),
              {
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

  it('includes keycloak client secret during token exchange when configured',
     async () => {
       const app = Fastify();
       await registerAuthRoutes(app);
       process.env.BANANA_KEYCLOAK_CLIENT_SECRET = 'secret-for-test';

       globalThis.fetch =
           mock(async (_input, init) => {
             const body = new URLSearchParams(String(init?.body ?? ''));
             expect(body.get('client_secret')).toBe('secret-for-test');
             return new Response(
                 JSON.stringify(
                     {id_token: makeUnsignedJwt({sub: 'secret-user'})}),
                 {
                   status: 200,
                   headers: {'content-type': 'application/json'},
                 });
           }) as unknown as typeof fetch;

       const response = await app.inject({
         method: 'GET',
         url: `/auth/keycloak/callback?code=auth-code-4b&state=${
             makeAuthState()}&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin`,
         headers: {
           host: 'api.banana.engineer',
         },
       });

       expect(response.statusCode).toBe(302);
       expect(response.headers.location ?? '')
           .toContain('subject=keycloak%3Asecret-user');

       delete process.env.BANANA_KEYCLOAK_CLIENT_SECRET;
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

describe('auth route helpers', () => {
  beforeEach(() => {
    mock.restore();
    delete process.env.BANANA_KEYCLOAK_ISSUER_URL;
    delete process.env.BANANA_KEYCLOAK_TOKEN_ISSUER_URL;
    delete process.env.BANANA_KEYCLOAK_CLIENT_ID;
    delete process.env.BANANA_KEYCLOAK_CLIENT_SECRET;
    delete process.env.BANANA_KEYCLOAK_ENV;
    delete process.env.BANANA_DEPLOY_ENV;
    delete process.env.FLY_APP_NAME;
    delete process.env.NODE_ENV;
  });

  it('covers query, origin, and returnTo normalization fallbacks', () => {
    expect(authRouteInternals.getFirstQueryValue([
      'first', 'second'
    ])).toBe('first');
    expect(authRouteInternals.resolveRequestOrigin({
      headers: {'x-forwarded-proto': 'https, http'},
      protocol: 'http',
    } as never))
        .toBe('https://localhost:8080');
    expect(authRouteInternals.normalizeReturnTo(undefined, '/login'))
        .toBe('/login');
    expect(
        authRouteInternals.normalizeReturnTo('ftp://banana.engineer', '/login'))
        .toBe('/login');
    expect(authRouteInternals.normalizeReturnTo('/profile', '/login'))
        .toBe('/profile');
    expect(authRouteInternals.normalizeReturnTo('not a url', '/login'))
        .toBe('/login');
  });

  it('covers keycloak env resolution helpers', () => {
    expect(authRouteInternals.resolveKeycloakIssuerUrl())
        .toBe('https://banana-keycloak-dev.fly.dev/realms/banana');
    expect(authRouteInternals.resolveKeycloakTokenIssuerUrl())
        .toBe('https://banana-keycloak-dev.fly.dev/realms/banana');
    expect(authRouteInternals.resolveKeycloakClientId())
        .toBe('banana-react-spa');
    expect(authRouteInternals.resolveKeycloakClientSecret()).toBe('');

    process.env.BANANA_KEYCLOAK_ISSUER_URL = ' https://issuer.example/realm ';
    process.env.BANANA_KEYCLOAK_TOKEN_ISSUER_URL =
        ' https://token.example/realm ';
    process.env.BANANA_KEYCLOAK_CLIENT_ID = ' banana-client ';
    process.env.BANANA_KEYCLOAK_CLIENT_SECRET = ' banana-secret ';

    expect(authRouteInternals.resolveKeycloakIssuerUrl())
        .toBe('https://issuer.example/realm');
    expect(authRouteInternals.resolveKeycloakTokenIssuerUrl())
        .toBe('https://token.example/realm');
    expect(authRouteInternals.resolveKeycloakClientId()).toBe('banana-client');
    expect(authRouteInternals.resolveKeycloakClientSecret())
        .toBe('banana-secret');
  });

  it('fails closed when dev runtime resolves production keycloak authority host',
     () => {
       process.env.NODE_ENV = 'development';
       process.env.BANANA_KEYCLOAK_ISSUER_URL =
           'https://kc-idp.banana.engineer/realms/banana';
       process.env.BANANA_KEYCLOAK_TOKEN_ISSUER_URL =
           'https://banana-keycloak-dev.fly.dev/realms/banana';

       expect(() => authRouteInternals.assertKeycloakAuthorityMapping())
           .toThrow(/keycloak_authority_mapping_invalid/);
     });

  it('allows production authority host outside dev runtime guard', () => {
    process.env.NODE_ENV = 'production';
    process.env.BANANA_KEYCLOAK_ISSUER_URL =
        'https://kc-idp.banana.engineer/realms/banana';
    process.env.BANANA_KEYCLOAK_TOKEN_ISSUER_URL =
        'https://kc-idp.banana.engineer/realms/banana';

    expect(() => authRouteInternals.assertKeycloakAuthorityMapping())
        .not.toThrow();
  });

  it('allows dev runtime when both issuer hosts are dev authority', () => {
    process.env.NODE_ENV = 'development';
    process.env.BANANA_KEYCLOAK_ISSUER_URL =
        'https://banana-keycloak-dev.fly.dev/realms/banana';
    process.env.BANANA_KEYCLOAK_TOKEN_ISSUER_URL =
        'https://banana-keycloak-dev.fly.dev/realms/banana';

    expect(() => authRouteInternals.assertKeycloakAuthorityMapping())
        .not.toThrow();
  });

  it('covers auth state decode error branches', () => {
    const valid = authRouteInternals.encodeKeycloakAuthState({
      nonce: 'nonce-1',
      codeVerifier: 'verifier-1',
    });
    expect(authRouteInternals.decodeKeycloakAuthState(valid)).toEqual({
      nonce: 'nonce-1',
      codeVerifier: 'verifier-1',
    });
    expect(authRouteInternals.decodeKeycloakAuthState('')).toBeNull();
    expect(authRouteInternals.decodeKeycloakAuthState(
               Buffer.from(JSON.stringify({nonce: 'nonce-only'}), 'utf8')
                   .toString('base64url')))
        .toBeNull();
    expect(authRouteInternals.decodeKeycloakAuthState('not-base64url'))
        .toBeNull();
  });

  it('covers provider, jwt subject, and identity normalization helpers', () => {
    expect(authRouteInternals.normalizeIdentityProvider('LINKEDIN'))
        .toBe('linkedin');
    expect(authRouteInternals.normalizeIdentityProvider('discord')).toBeNull();
    expect(authRouteInternals.normalizeAuthAction('REGISTER')).toBe('register');
    expect(authRouteInternals.normalizeAuthAction('reset-password'))
        .toBe('UPDATE_PASSWORD');
    expect(authRouteInternals.normalizeAuthAction('login')).toBeNull();

    expect(authRouteInternals.resolveSubjectFromJwt(undefined)).toBeNull();
    expect(authRouteInternals.resolveSubjectFromJwt(makeUnsignedJwt({sub: ''})))
        .toBeNull();
    expect(authRouteInternals.resolveSubjectFromJwt(makeUnsignedJwt({
      sub: 'steam:abc'
    }))).toBe('steam:abc');
    expect(authRouteInternals.resolveSubjectFromJwt('not-a-jwt')).toBeNull();

    expect(authRouteInternals.resolveIdentityIdFromSubject('guest:abc'))
        .toBe('guest:abc');
    expect(authRouteInternals.resolveIdentityIdFromSubject('plain-subject'))
        .toBe('keycloak:plain-subject');
    expect(authRouteInternals.resolveIdentityKind('guest:abc')).toBe('guest');
    expect(authRouteInternals.resolveIdentityKind('keycloak:abc'))
        .toBe('keycloak');
  });

  it('covers bearer parsing, expiry fallback, and guest alias normalization',
     () => {
       expect(authRouteInternals.parseBearerToken(undefined)).toBeNull();
       expect(authRouteInternals.parseBearerToken('Basic abc')).toBeNull();
       expect(authRouteInternals.parseBearerToken('Bearer   ')).toBeNull();
       expect(authRouteInternals.parseBearerToken('Bearer token-123'))
           .toBe('token-123');

       const expDate = authRouteInternals.resolveTokenExpiry(
           makeUnsignedJwt({sub: 'user-1', exp: 1735689600}));
       expect(expDate.toISOString()).toBe('2025-01-01T00:00:00.000Z');

       const fallbackDate = authRouteInternals.resolveTokenExpiry('not-a-jwt');
       expect(fallbackDate instanceof Date).toBe(true);

       expect(authRouteInternals.normalizeGuestAlias(undefined)).toBe('Guest');
       expect(authRouteInternals.normalizeGuestAlias('    ')).toBe('Guest');
       expect(authRouteInternals.normalizeGuestAlias('!!!')).toBe('Guest');
     });
});
