import type {FastifyInstance, FastifyReply, FastifyRequest} from 'fastify';
import * as jose from 'jose';
import {createHash, randomUUID} from 'node:crypto';

import {signToken, verifyToken} from '../middleware/auth.ts';
import {deriveDefaultSessionExpiry, getAuthSessionStore,} from '../services/authSessionStore.ts';

const KEYCLOAK_DEFAULT_ISSUER = 'http://localhost:8080/realms/banana';
const KEYCLOAK_DEFAULT_CLIENT_ID = 'banana-web';
const KEYCLOAK_CALLBACK_PATH = '/auth/keycloak/callback';
const DEFAULT_LOGIN_RETURN_TO = '/login';

type QueryValue = string|string[]|undefined;

type AuthQuery = {
  [key: string]: QueryValue;
  returnTo?: QueryValue;
  subject?: QueryValue;
  sub?: QueryValue;
  preferred_username?: QueryValue;
  code?: QueryValue;
};

type GuestStartBody = {
  alias?: string;
};

function getFirstQueryValue(value: QueryValue): string {
  if (Array.isArray(value)) {
    return value[0] ?? '';
  }

  return value ?? '';
}

function resolveRequestOrigin(request: FastifyRequest): string {
  const host = request.headers.host?.trim();
  const forwardedProto =
      getFirstQueryValue(
          request.headers['x-forwarded-proto'] as QueryValue)
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

function resolveKeycloakIssuerUrl(): string {
  const configured = (process.env.BANANA_KEYCLOAK_ISSUER_URL ?? '').trim();
  return configured || KEYCLOAK_DEFAULT_ISSUER;
}

function resolveKeycloakClientId(): string {
  const configured = (process.env.BANANA_KEYCLOAK_CLIENT_ID ?? '').trim();
  return configured || KEYCLOAK_DEFAULT_CLIENT_ID;
}

function buildKeycloakAuthUrl(origin: string, returnTo: string): string {
  const callbackUrl = new URL(KEYCLOAK_CALLBACK_PATH, origin);
  callbackUrl.searchParams.set('returnTo', returnTo);

  const issuerUrl = resolveKeycloakIssuerUrl();
  const issuerBase = issuerUrl.endsWith('/') ? issuerUrl.slice(0, -1) : issuerUrl;
  const authUrl = new URL(`${issuerBase}/protocol/openid-connect/auth`);
  authUrl.searchParams.set('client_id', resolveKeycloakClientId());
  authUrl.searchParams.set('response_type', 'code');
  authUrl.searchParams.set('scope', 'openid profile email');
  authUrl.searchParams.set('redirect_uri', callbackUrl.toString());
  authUrl.searchParams.set('state', randomUUID());

  return authUrl.toString();
}

function resolveIdentitySubject(query: AuthQuery): string|null {
  const explicitSubject =
      getFirstQueryValue(query.subject ?? query.sub ?? query.preferred_username)
          .trim();
  if (explicitSubject) {
    return explicitSubject.includes(':') ? explicitSubject :
                                           `keycloak:${explicitSubject}`;
  }

  const code = getFirstQueryValue(query.code).trim();
  if (!code) {
    return null;
  }

  const codeSuffix = code.slice(0, 24).replace(/[^a-zA-Z0-9_-]/g, '');
  return `keycloak:${codeSuffix || 'user'}`;
}

function resolveIdentityIdFromSubject(subject: string): string {
  const normalized = subject.trim();
  if (normalized.startsWith('steam:') || normalized.startsWith('keycloak:') ||
      normalized.startsWith('guest:')) {
    return normalized;
  }

  return `keycloak:${normalized}`;
}

function resolveIdentityKind(subject: string): 'guest'|'keycloak' {
  return subject.startsWith('guest:') ? 'guest' : 'keycloak';
}

function buildCallbackRedirect(
    returnTo: string, token: string, subject: string): string {
  const redirectUrl = new URL(
      returnTo, returnTo.startsWith('/') ? 'http://localhost:5173' : undefined);
  const hash = new URLSearchParams({
    auth_token: token,
    subject,
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

function normalizeGuestAlias(rawAlias: string|undefined): string {
  const value = (rawAlias ?? '').trim().replace(/\s+/g, ' ');
  if (!value) {
    return 'Guest';
  }

  const normalized = value.replace(/[^a-zA-Z0-9 _-]/g, '').slice(0, 24).trim();
  return normalized.length > 0 ? normalized : 'Guest';
}

export async function registerAuthRoutes(app: FastifyInstance) {
  const authStore = await getAuthSessionStore();

  app.post('/auth/guest/start', async (request, reply) => {
    const body = (request.body ?? {}) as GuestStartBody;
    const alias = normalizeGuestAlias(body.alias);
    const guestId = randomUUID().replace(/-/g, '').slice(0, 16);
    const subject = `guest:${guestId}`;

    const token = await signToken({sub: subject, role: 'viewer'});
    const tokenHash = hashToken(token);
    const expiresAt = resolveTokenExpiry(token);

    await authStore.upsertIdentityUser(subject);
    await authStore.createIdentitySession({
      identityId: subject,
      tokenHash,
      expiresAt,
    });

    return reply.send({
      token,
      guestId,
      displayName: alias,
      expiresAt: expiresAt.toISOString(),
    });
  });

  app.get('/auth/keycloak/start', async (request, reply) => {
    const origin = resolveRequestOrigin(request);
    const query = request.query as AuthQuery;
    const returnTo = normalizeReturnTo(
        getFirstQueryValue(query.returnTo) || undefined,
        `${origin}${DEFAULT_LOGIN_RETURN_TO}`);
    return reply.redirect(buildKeycloakAuthUrl(origin, returnTo), 302);
  });

  app.get('/auth/keycloak/callback', async (request, reply) => {
    const query = request.query as AuthQuery;
    const resolvedSubject = resolveIdentitySubject(query);
    if (!resolvedSubject) {
      return reply.status(400).send({error: 'keycloak_auth_not_completed'});
    }

    const identitySubject = resolveIdentityIdFromSubject(resolvedSubject);
    const token = await signToken({sub: identitySubject, role: 'viewer'});
    const tokenHash = hashToken(token);
    const expiresAt = resolveTokenExpiry(token);

    await authStore.upsertIdentityUser(identitySubject);
    await authStore.createIdentitySession({
      identityId: identitySubject,
      tokenHash,
      expiresAt,
    });

    const origin = resolveRequestOrigin(request);
    const returnTo = normalizeReturnTo(
        getFirstQueryValue(query.returnTo) || undefined,
        `${origin}${DEFAULT_LOGIN_RETURN_TO}`);

    return reply.redirect(
        buildCallbackRedirect(returnTo, token, identitySubject), 302);
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
      identityId: resolveIdentityIdFromSubject(claims.sub),
      guestId: claims.sub.startsWith('guest:') ? claims.sub.slice(6) : null,
      identityKind: resolveIdentityKind(claims.sub),
      subject: claims.sub,
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
