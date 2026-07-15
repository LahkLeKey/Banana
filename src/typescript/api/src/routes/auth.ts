import type {FastifyInstance, FastifyReply, FastifyRequest} from 'fastify';
import * as jose from 'jose';
import {createHash, randomBytes, randomUUID} from 'node:crypto';

import {signToken, verifyToken} from '../middleware/auth.ts';
import {deriveDefaultSessionExpiry, getAuthSessionStore,} from '../services/authSessionStore.ts';

const KEYCLOAK_DEFAULT_ISSUER =
    'https://banana-keycloak-dev.fly.dev/realms/banana';
const KEYCLOAK_DEFAULT_CLIENT_ID = 'banana-react-spa';
const KEYCLOAK_CALLBACK_PATH = '/auth/keycloak/callback';
const KEYCLOAK_DEV_AUTHORITY_HOST = 'banana-keycloak-dev.fly.dev';
const KEYCLOAK_PRODUCTION_AUTHORITY_HOST = 'kc-idp.banana.engineer';
const DEFAULT_LOGIN_RETURN_TO = '/login';

type QueryValue = string|string[]|undefined;

type AuthQuery = {
  [key: string]: QueryValue;
  returnTo?: QueryValue;
  code?: QueryValue;
  provider?: QueryValue;
  action?: QueryValue;
};

type KeycloakTokenResponse = {
  access_token?: string;
  id_token?: string;
};

type KeycloakTokenExchangeError = {
  status: number; body: string;
};

type KeycloakAuthState = {
  nonce: string; codeVerifier: string;
};

type GuestStartBody = {
  alias?: string;
};

type KeycloakAuthAction = 'register'|'UPDATE_PASSWORD';

function getFirstQueryValue(value: QueryValue): string {
  if (Array.isArray(value)) {
    return value[0] ?? '';
  }

  return value ?? '';
}

function resolveRequestOrigin(request: FastifyRequest): string {
  const host = request.headers.host?.trim();
  const forwardedProto =
      getFirstQueryValue(request.headers['x-forwarded-proto'] as QueryValue)
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

function resolveKeycloakTokenIssuerUrl(): string {
  const configured =
      (process.env.BANANA_KEYCLOAK_TOKEN_ISSUER_URL ?? '').trim();
  return configured || resolveKeycloakIssuerUrl();
}

function resolveKeycloakClientId(): string {
  const configured = (process.env.BANANA_KEYCLOAK_CLIENT_ID ?? '').trim();
  return configured || KEYCLOAK_DEFAULT_CLIENT_ID;
}

function resolveKeycloakClientSecret(): string {
  return (process.env.BANANA_KEYCLOAK_CLIENT_SECRET ?? '').trim();
}

function normalizeEnvValue(rawValue: string|undefined): string {
  return (rawValue ?? '').trim().toLowerCase();
}

function isDevRuntimeForKeycloakGuard(): boolean {
  const keycloakEnv = normalizeEnvValue(process.env.BANANA_KEYCLOAK_ENV);
  if (keycloakEnv === 'dev' || keycloakEnv === 'development' ||
      keycloakEnv === 'local') {
    return true;
  }

  const deployEnv = normalizeEnvValue(process.env.BANANA_DEPLOY_ENV);
  if (deployEnv === 'dev' || deployEnv === 'development' ||
      deployEnv === 'local') {
    return true;
  }

  const nodeEnv = normalizeEnvValue(process.env.NODE_ENV);
  if (nodeEnv && nodeEnv !== 'production') {
    return true;
  }

  const flyAppName = normalizeEnvValue(process.env.FLY_APP_NAME);
  if (flyAppName && flyAppName !== 'banana-api') {
    return true;
  }

  return nodeEnv.length === 0;
}

function isProductionRuntimeForKeycloakGuard(): boolean {
  const keycloakEnv = normalizeEnvValue(process.env.BANANA_KEYCLOAK_ENV);
  if (keycloakEnv === 'prod' || keycloakEnv === 'production') {
    return true;
  }

  const deployEnv = normalizeEnvValue(process.env.BANANA_DEPLOY_ENV);
  if (deployEnv === 'prod' || deployEnv === 'production') {
    return true;
  }

  const nodeEnv = normalizeEnvValue(process.env.NODE_ENV);
  if (nodeEnv === 'production') {
    return true;
  }

  const flyAppName = normalizeEnvValue(process.env.FLY_APP_NAME);
  return flyAppName === 'banana-api';
}
function resolveAuthorityHost(rawUrl: string): string {
  try {
    return new URL(rawUrl).hostname.trim().toLowerCase();
  } catch {
    return '';
  }
}

function assertKeycloakAuthorityMapping(): void {
  const issuerUrl = resolveKeycloakIssuerUrl();
  const tokenIssuerUrl = resolveKeycloakTokenIssuerUrl();
  const issuerHost = resolveAuthorityHost(issuerUrl);
  const tokenIssuerHost = resolveAuthorityHost(tokenIssuerUrl);

  if (isProductionRuntimeForKeycloakGuard()) {
    if (issuerHost === KEYCLOAK_PRODUCTION_AUTHORITY_HOST &&
        tokenIssuerHost === KEYCLOAK_PRODUCTION_AUTHORITY_HOST) {
      return;
    }

    throw new Error(
        `keycloak_authority_mapping_invalid: production runtime requires ${KEYCLOAK_PRODUCTION_AUTHORITY_HOST}; found ${issuerHost || 'missing'} and ${tokenIssuerHost || 'missing'} in BANANA_KEYCLOAK_ISSUER_URL and BANANA_KEYCLOAK_TOKEN_ISSUER_URL`);
  }

  if (!isDevRuntimeForKeycloakGuard()) {
    return;
  }
  if (issuerHost !== KEYCLOAK_PRODUCTION_AUTHORITY_HOST &&
      tokenIssuerHost !== KEYCLOAK_PRODUCTION_AUTHORITY_HOST) {
    return;
  }

  throw new Error(
      `keycloak_authority_mapping_invalid: dev runtime requires ${KEYCLOAK_DEV_AUTHORITY_HOST}; found production authority ${KEYCLOAK_PRODUCTION_AUTHORITY_HOST} in BANANA_KEYCLOAK_ISSUER_URL or BANANA_KEYCLOAK_TOKEN_ISSUER_URL`);
}

function generatePkceCodeVerifier(): string {
  return randomBytes(32).toString('base64url');
}

function derivePkceCodeChallenge(codeVerifier: string): string {
  return createHash('sha256').update(codeVerifier).digest('base64url');
}

function encodeKeycloakAuthState(state: KeycloakAuthState): string {
  return Buffer.from(JSON.stringify(state), 'utf8').toString('base64url');
}

function decodeKeycloakAuthState(rawState: string): KeycloakAuthState|null {
  const value = rawState.trim();
  if (!value) {
    return null;
  }

  try {
    const parsed = JSON.parse(Buffer.from(value, 'base64url').toString('utf8'));
    const nonce = typeof parsed?.nonce === 'string' ? parsed.nonce.trim() : '';
    const codeVerifier = typeof parsed?.codeVerifier === 'string' ?
        parsed.codeVerifier.trim() :
        '';
    if (!nonce || !codeVerifier) {
      return null;
    }

    return {nonce, codeVerifier};
  } catch {
    return null;
  }
}

function buildKeycloakAuthUrl(
    origin: string, returnTo: string, codeVerifier: string,
    action?: KeycloakAuthAction|null): string {
  const callbackUrl = new URL(KEYCLOAK_CALLBACK_PATH, origin);
  callbackUrl.searchParams.set('returnTo', returnTo);

  const issuerUrl = resolveKeycloakIssuerUrl();
  const issuerBase =
      issuerUrl.endsWith('/') ? issuerUrl.slice(0, -1) : issuerUrl;
  const authUrl = new URL(`${issuerBase}/protocol/openid-connect/auth`);
  authUrl.searchParams.set('client_id', resolveKeycloakClientId());
  authUrl.searchParams.set('response_type', 'code');
  authUrl.searchParams.set('scope', 'openid profile email');
  authUrl.searchParams.set('redirect_uri', callbackUrl.toString());
  authUrl.searchParams.set('state', encodeKeycloakAuthState({
                             nonce: randomUUID(),
                             codeVerifier,
                           }));
  authUrl.searchParams.set(
      'code_challenge', derivePkceCodeChallenge(codeVerifier));
  authUrl.searchParams.set('code_challenge_method', 'S256');
  if (action) {
    authUrl.searchParams.set('kc_action', action);
  }

  return authUrl.toString();
}

function normalizeIdentityProvider(rawProvider: string): 'github'|'google'|
    'linkedin'|null {
  const normalized = rawProvider.trim().toLowerCase();
  if (normalized === 'github' || normalized === 'google' ||
      normalized === 'linkedin') {
    return normalized;
  }

  return null;
}

function normalizeAuthAction(rawAction: string): KeycloakAuthAction|null {
  const normalized = rawAction.trim().toLowerCase();
  if (normalized === 'register') {
    return 'register';
  }
  if (normalized === 'reset-password' || normalized === 'update-password') {
    return 'UPDATE_PASSWORD';
  }

  return null;
}

function resolveKeycloakTokenEndpoint(): string {
  const issuerUrl = resolveKeycloakTokenIssuerUrl();
  const issuerBase =
      issuerUrl.endsWith('/') ? issuerUrl.slice(0, -1) : issuerUrl;
  return `${issuerBase}/protocol/openid-connect/token`;
}

function resolveSubjectFromJwt(rawToken: string|undefined): string|null {
  const token = (rawToken ?? '').trim();
  if (!token) {
    return null;
  }

  try {
    const payload = jose.decodeJwt(token);
    const subject = (typeof payload.sub === 'string' ? payload.sub : '').trim();
    if (!subject) {
      return null;
    }

    return subject.includes(':') ? subject : `keycloak:${subject}`;
  } catch {
    return null;
  }
}

async function exchangeKeycloakAuthorizationCode(
    code: string, redirectUri: string,
    codeVerifier: string): Promise<KeycloakTokenResponse> {
  const body = new URLSearchParams({
    grant_type: 'authorization_code',
    code,
    client_id: resolveKeycloakClientId(),
    redirect_uri: redirectUri,
    code_verifier: codeVerifier,
  });

  const clientSecret = resolveKeycloakClientSecret();
  if (clientSecret) {
    body.set('client_secret', clientSecret);
  }

  const response = await fetch(resolveKeycloakTokenEndpoint(), {
    method: 'POST',
    headers: {
      'content-type': 'application/x-www-form-urlencoded',
    },
    body,
  });

  if (!response.ok) {
    const bodyText = await response.text();
    const error = new Error('keycloak_token_exchange_failed') as Error &
        Partial<KeycloakTokenExchangeError>;
    error.status = response.status;
    error.body = bodyText;
    throw error;
  }

  return await response.json() as KeycloakTokenResponse;
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

export const authRouteInternals = {
  getFirstQueryValue,
  resolveRequestOrigin,
  normalizeReturnTo,
  resolveKeycloakIssuerUrl,
  resolveKeycloakTokenIssuerUrl,
  resolveKeycloakClientId,
  resolveKeycloakClientSecret,
  assertKeycloakAuthorityMapping,
  encodeKeycloakAuthState,
  decodeKeycloakAuthState,
  normalizeIdentityProvider,
  normalizeAuthAction,
  resolveSubjectFromJwt,
  resolveIdentityIdFromSubject,
  resolveIdentityKind,
  parseBearerToken,
  resolveTokenExpiry,
  normalizeGuestAlias,
};

export async function registerAuthRoutes(app: FastifyInstance) {
  assertKeycloakAuthorityMapping();
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
    const provider =
        normalizeIdentityProvider(getFirstQueryValue(query.provider));
    const action = normalizeAuthAction(getFirstQueryValue(query.action));
    const codeVerifier = generatePkceCodeVerifier();

    const redirectUrl =
        new URL(buildKeycloakAuthUrl(origin, returnTo, codeVerifier, action));
    if (provider) {
      redirectUrl.searchParams.set('kc_idp_hint', provider);
    }

    return reply.redirect(redirectUrl.toString(), 302);
  });

  app.get('/auth/keycloak/callback', async (request, reply) => {
    const query = request.query as AuthQuery;
    const code = getFirstQueryValue(query.code).trim();
    if (!code) {
      return reply.status(400).send({error: 'keycloak_auth_not_completed'});
    }

    const origin = resolveRequestOrigin(request);
    const returnTo = normalizeReturnTo(
        getFirstQueryValue(query.returnTo) || undefined,
        `${origin}${DEFAULT_LOGIN_RETURN_TO}`);
    const callbackUrl = new URL(KEYCLOAK_CALLBACK_PATH, origin);
    callbackUrl.searchParams.set('returnTo', returnTo);
    const authState = decodeKeycloakAuthState(getFirstQueryValue(query.state));
    if (!authState) {
      return reply.status(400).send({error: 'keycloak_auth_state_missing'});
    }

    let tokenResponse: KeycloakTokenResponse;
    try {
      tokenResponse = await exchangeKeycloakAuthorizationCode(
          code, callbackUrl.toString(), authState.codeVerifier);
    } catch (error) {
      const failure = error as Error & Partial<KeycloakTokenExchangeError>;
      return reply.status(401).send({
        error: 'keycloak_token_exchange_failed',
        status: failure.status ?? null,
        detail: failure.body ?? null,
      });
    }

    const resolvedSubject = resolveSubjectFromJwt(tokenResponse.id_token) ??
        resolveSubjectFromJwt(tokenResponse.access_token);
    if (!resolvedSubject) {
      return reply.status(401).send({error: 'keycloak_identity_not_available'});
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
