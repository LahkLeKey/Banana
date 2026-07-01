export const BANANA_AUTH_TOKEN_STORAGE_KEY = 'banana-auth-token';
export const BANANA_AUTH_SUBJECT_STORAGE_KEY = 'banana-auth-subject';

let activeAuthSession: AuthSession|null = null;

export type AuthSession = {
  token: string; subject: string;
};

export type AuthProvider = 'github'|'google'|'linkedin';

function normalizeHash(rawHash: string): URLSearchParams {
  const value = rawHash.startsWith('#') ? rawHash.slice(1) : rawHash;
  return new URLSearchParams(value);
}

export function parseAuthCallbackHash(rawHash: string): AuthSession|null {
  const params = normalizeHash(rawHash);
  const token = (params.get('auth_token') ?? params.get('token') ?? '').trim();
  if (!token) {
    return null;
  }

  const subject =
      (params.get('subject') ?? params.get('sub') ?? params.get('player_id') ??
       params.get('steam_id') ?? params.get('steamId') ?? 'authenticated-user')
          .trim();
  return {
    token,
    subject,
  };
}

export function readStoredAuthSession(): AuthSession|null {
  if (!activeAuthSession?.token) {
    return null;
  }

  return {
    token: activeAuthSession.token,
    subject: activeAuthSession.subject,
  };
}

export function hasStoredAuthSession(): boolean {
  return readStoredAuthSession() !== null;
}

export function storeAuthSession(session: AuthSession): void {
  activeAuthSession = {
    token: session.token.trim(),
    subject: session.subject.trim(),
  };
}

export function clearStoredAuthSession(): void {
  activeAuthSession = null;
}

export function resolveLoginReturnToUrl(): string {
  if (typeof window === 'undefined') {
    return 'http://localhost:5173/login';
  }

  return `${window.location.origin}/login`;
}

export function buildAuthStartUrl(
    apiBaseUrl: string, returnTo: string, provider?: AuthProvider): string {
  const url = new URL('/auth/keycloak/start', apiBaseUrl);
  url.searchParams.set('returnTo', returnTo);
  if (provider) {
    url.searchParams.set('provider', provider);
  }
  return url.toString();
}

export const buildSteamAuthStartUrl = buildAuthStartUrl;

export async function validateAuthSession(
    apiBaseUrl: string, token: string): Promise<boolean> {
  if (!token.trim() || !apiBaseUrl.trim()) {
    return false;
  }

  try {
    const response = await fetch(`${apiBaseUrl}/auth/session`, {
      method: 'GET',
      headers: {
        authorization: `Bearer ${token}`,
      },
    });
    return response.ok;
  } catch {
    return false;
  }
}

export async function logoutAuthSession(
    apiBaseUrl: string, token: string): Promise<void> {
  if (!token.trim() || !apiBaseUrl.trim()) {
    return;
  }

  try {
    await fetch(`${apiBaseUrl}/auth/logout`, {
      method: 'POST',
      headers: {
        authorization: `Bearer ${token}`,
      },
    });
  } catch {
    // Best-effort logout to keep local sign-out non-blocking.
  }
}
