import type {FastifyInstance, FastifyReply, FastifyRequest} from 'fastify';
import * as jose from 'jose';
import {createHash} from 'node:crypto';

import {signToken, verifyToken} from '../middleware/auth.ts';
import {deriveDefaultSessionExpiry, getAuthSessionStore,} from '../services/authSessionStore.ts';

const STEAM_OPENID_ENDPOINT = 'https://steamcommunity.com/openid/login';
const STEAM_OPENID_NAMESPACE = 'http://specs.openid.net/auth/2.0';
const STEAM_OPENID_IDENTITY =
    'http://specs.openid.net/auth/2.0/identifier_select';
const STEAM_CALLBACK_PATH = '/auth/steam/callback';
const DEFAULT_LOGIN_RETURN_TO = '/login';

type SteamQueryValue = string|string[]|undefined;

type SteamOpenIdQuery = {
  [key: string]: SteamQueryValue;
  returnTo?: SteamQueryValue;
};

function getFirstQueryValue(value: SteamQueryValue): string {
  if (Array.isArray(value)) {
    return value[0] ?? '';
  }

  return value ?? '';
}

function resolveRequestOrigin(request: FastifyRequest): string {
  const host = request.headers.host?.trim();
  const forwardedProto =
      getFirstQueryValue(
          request.headers['x-forwarded-proto'] as SteamQueryValue)
          .trim();
  const protocol =
      (forwardedProto || request.protocol || 'http').split(',')[0].trim();

  if (!host) {
    return `${protocol}://localhost:8080`;
  }

  return `${protocol}://${host}`;
}

function normalizeReturnTo(raw: string|undefined, fallback: string): string {
  const candidate = (raw ?? '').trim();
  if (!candidate) {
    return fallback;
  }

  try {
    const parsed = new URL(candidate);
    if (parsed.protocol !== 'http:' && parsed.protocol !== 'https:') {
      return fallback;
    }
    parsed.username = '';
    parsed.password = '';
    return parsed.toString();
  } catch {
    if (candidate.startsWith('/')) {
      return candidate;
    }
    return fallback;
  }
}

function buildSteamOpenIdUrl(origin: string, returnTo: string): string {
  const callbackUrl = new URL(STEAM_CALLBACK_PATH, origin);
  callbackUrl.searchParams.set('returnTo', returnTo);

  const url = new URL(STEAM_OPENID_ENDPOINT);
  url.searchParams.set('openid.ns', STEAM_OPENID_NAMESPACE);
  url.searchParams.set('openid.mode', 'checkid_setup');
  url.searchParams.set('openid.claimed_id', STEAM_OPENID_IDENTITY);
  url.searchParams.set('openid.identity', STEAM_OPENID_IDENTITY);
  url.searchParams.set('openid.return_to', callbackUrl.toString());
  url.searchParams.set('openid.realm', origin);
  return url.toString();
}

function extractSteamId(claimedId: string): string|null {
  const match =
      claimedId.match(/^https:\/\/steamcommunity\.com\/openid\/id\/(\d+)$/);
  return match?.[1] ?? null;
}

async function verifySteamOpenId(requestPayload: URLSearchParams):
    Promise<boolean> {
  const payload = new URLSearchParams(requestPayload);
  payload.set('openid.mode', 'check_authentication');

  const response = await fetch(STEAM_OPENID_ENDPOINT, {
    method: 'POST',
    headers: {
      'content-type': 'application/x-www-form-urlencoded',
    },
    body: payload,
  });

  if (!response.ok) {
    return false;
  }

  const text = await response.text();
  return /is_valid\s*:\s*true/i.test(text);
}

function buildCallbackRedirect(
    returnTo: string, token: string, steamId: string): string {
  const redirectUrl = new URL(
      returnTo, returnTo.startsWith('/') ? 'http://localhost:5173' : undefined);
  const hash = new URLSearchParams({
    auth_token: token,
    steam_id: steamId,
  });
  redirectUrl.hash = hash.toString();
  return redirectUrl.toString();
}

function parseBearerToken(authHeaderRaw: string|undefined): string|null {
  const authHeader = (authHeaderRaw ?? '').trim();
  if (!authHeader.startsWith('Bearer ')) {
    return null;
  }

  const token = authHeader.slice(7).trim();
  return token.length > 0 ? token : null;
}

function hashToken(token: string): string {
  return createHash('sha256').update(token).digest('hex');
}

function resolveTokenExpiry(token: string): Date {
  try {
    const payload = jose.decodeJwt(token);
    if (typeof payload.exp === 'number' && Number.isFinite(payload.exp)) {
      return new Date(payload.exp * 1000);
    }
  } catch {
    // Use default duration when token decode fails.
  }

  return deriveDefaultSessionExpiry();
}

function resolveSteamIdFromSubject(subject: string): string {
  return subject.startsWith('steam:') ? subject.slice(6) : subject;
}

export async function registerAuthRoutes(app: FastifyInstance) {
  const authStore = await getAuthSessionStore();

  app.get('/auth/steam/start', async (request, reply) => {
    const origin = resolveRequestOrigin(request);
    const query = request.query as SteamOpenIdQuery;
    const returnTo = normalizeReturnTo(
        getFirstQueryValue(query.returnTo) || undefined,
        `${origin}${DEFAULT_LOGIN_RETURN_TO}`);
    return reply.redirect(buildSteamOpenIdUrl(origin, returnTo), 302);
  });

  app.get('/auth/steam/callback', async (request, reply) => {
    const query = request.query as SteamOpenIdQuery;
    const payload = new URLSearchParams();

    for (const [key, value] of Object.entries(query)) {
      const first = getFirstQueryValue(value);
      if (first.length > 0) {
        payload.set(key, first);
      }
    }

    const mode = payload.get('openid.mode') ?? '';
    if (mode !== 'id_res') {
      return reply.status(400).send({error: 'steam_openid_not_completed'});
    }

    const claimedId = payload.get('openid.claimed_id') ??
        payload.get('openid.identity') ?? '';
    const steamId = extractSteamId(claimedId);
    if (!steamId) {
      return reply.status(400).send({error: 'invalid_steam_claimed_id'});
    }

    const isValid = await verifySteamOpenId(payload);
    if (!isValid) {
      return reply.status(401).send(
          {error: 'steam_openid_verification_failed'});
    }

    const token = await signToken({sub: `steam:${steamId}`, role: 'viewer'});
    const tokenHash = hashToken(token);
    const expiresAt = resolveTokenExpiry(token);

    await authStore.upsertSteamUser(steamId);
    await authStore.createSession({
      steamId,
      tokenHash,
      expiresAt,
    });

    const origin = resolveRequestOrigin(request);
    const returnTo = normalizeReturnTo(
        getFirstQueryValue(query.returnTo) || undefined,
        `${origin}${DEFAULT_LOGIN_RETURN_TO}`);

    return reply.redirect(buildCallbackRedirect(returnTo, token, steamId), 302);
  });

  app.get('/auth/session', async (request, reply) => {
    const token = parseBearerToken(request.headers.authorization);
    if (!token) {
      return reply.status(401).send({error: 'missing_bearer_token'});
    }

    let claims: Awaited<ReturnType<typeof verifyToken>>;
    try {
      claims = await verifyToken(token);
    } catch {
      return reply.status(401).send({error: 'invalid_or_expired_token'});
    }

    const active = await authStore.touchActiveSessionByTokenHash(
        hashToken(token), new Date());
    if (!active) {
      return reply.status(401).send({error: 'session_not_active'});
    }

    return reply.send({
      authenticated: true,
      role: claims.role,
      steamId: resolveSteamIdFromSubject(claims.sub),
      sub: claims.sub,
    });
  });

  app.post('/auth/logout', async (request, reply) => {
    const token = parseBearerToken(request.headers.authorization);
    if (!token) {
      return reply.status(401).send({error: 'missing_bearer_token'});
    }

    try {
      await verifyToken(token);
    } catch {
      return reply.status(401).send({error: 'invalid_or_expired_token'});
    }

    const revoked =
        await authStore.revokeSessionByTokenHash(hashToken(token), new Date());

    return reply.send({revoked});
  });
}
