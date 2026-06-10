import {beforeEach, describe, expect, it, mock} from 'bun:test';
import Fastify from 'fastify';
import type {FastifyInstance} from 'fastify';

import {registerAuthRoutes} from './auth.ts';

process.env.BANANA_JWT_SECRET = 'test-secret-banana-jwt-1234';

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

async function issueSteamToken(app: FastifyInstance): Promise<string> {
  const fetchMock = mock(async () => {
    return new Response(
        'ns:http://specs.openid.net/auth/2.0\nis_valid:true\n', {
          status: 200,
          headers: {'content-type': 'text/plain'},
        });
  });
  globalThis.fetch = fetchMock as unknown as typeof fetch;

  const response = await app.inject({
    method: 'GET',
    url:
        '/auth/steam/callback?openid.mode=id_res&openid.claimed_id=https%3A%2F%2Fsteamcommunity.com%2Fopenid%2Fid%2F76561198000000000&openid.identity=https%3A%2F%2Fsteamcommunity.com%2Fopenid%2Fid%2F76561198000000000&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
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

  it('redirects Steam start requests to the OpenID endpoint', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);

    const response = await app.inject({
      method: 'GET',
      url: '/auth/steam/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
      headers: {
        host: 'api.banana.engineer',
      },
    });

    expect(response.statusCode).toBe(302);
    expect(response.headers.location ?? '')
        .toContain('https://steamcommunity.com/openid/login');
    expect(response.headers.location ?? '')
        .toContain('openid.mode=checkid_setup');
    expect(response.headers.location ?? '')
        .toContain(
            'openid.return_to=http%3A%2F%2Fapi.banana.engineer%2Fauth%2Fsteam%2Fcallback%3FreturnTo%3Dhttps%253A%252F%252Fbanana.engineer%252Flogin');

    await app.close();
  });

  it('redirects verified callbacks back to the login return url with a token hash',
     async () => {
       const app = Fastify();
       await registerAuthRoutes(app);

       const fetchMock = mock(async () => {
         return new Response(
             'ns:http://specs.openid.net/auth/2.0\nis_valid:true\n', {
               status: 200,
               headers: {'content-type': 'text/plain'},
             });
       });
       globalThis.fetch = fetchMock as unknown as typeof fetch;

       const response = await app.inject({
         method: 'GET',
         url:
             '/auth/steam/callback?openid.mode=id_res&openid.claimed_id=https%3A%2F%2Fsteamcommunity.com%2Fopenid%2Fid%2F76561198000000000&openid.identity=https%3A%2F%2Fsteamcommunity.com%2Fopenid%2Fid%2F76561198000000000&returnTo=https%3A%2F%2Fbanana.engineer%2Flogin',
         headers: {
           host: 'api.banana.engineer',
         },
       });

       expect(response.statusCode).toBe(302);
       expect(response.headers.location ?? '')
           .toContain('https://banana.engineer/login#');
       expect(response.headers.location ?? '').toContain('auth_token=');
       expect(response.headers.location ?? '')
           .toContain('steam_id=76561198000000000');

       await app.close();
     });

  it('validates active auth sessions from bearer token', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);
    const token = await issueSteamToken(app);

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
      steamId: string;
    };
    expect(body.authenticated).toBe(true);
    expect(body.steamId).toBe('76561198000000000');
    expect(body.identityKind).toBe('steam');

    await app.close();
  });

  it('revokes a session and rejects it after logout', async () => {
    const app = Fastify();
    await registerAuthRoutes(app);
    const token = await issueSteamToken(app);

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
