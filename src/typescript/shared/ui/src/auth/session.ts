export const BANANA_AUTH_TOKEN_STORAGE_KEY = 'banana-auth-token';
export const BANANA_AUTH_STEAM_ID_STORAGE_KEY = 'banana-auth-steam-id';

export type AuthSession = {
  token: string; steamId: string;
};

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

  const steamId =
      (params.get('steam_id') ?? params.get('steamId') ?? '').trim();
  return {
    token,
    steamId,
  };
}

export function readStoredAuthSession(): AuthSession|null {
  if (typeof window === 'undefined') {
    return null;
  }

  const token =
      window.localStorage.getItem(BANANA_AUTH_TOKEN_STORAGE_KEY)?.trim() ?? '';
  if (!token) {
    return null;
  }

  const steamId =
      window.localStorage.getItem(BANANA_AUTH_STEAM_ID_STORAGE_KEY)?.trim() ??
      '';
  return {
    token,
    steamId,
  };
}

export function hasStoredAuthSession(): boolean {
  return readStoredAuthSession() !== null;
}

export function storeAuthSession(session: AuthSession): void {
  if (typeof window === 'undefined') {
    return;
  }

  window.localStorage.setItem(
      BANANA_AUTH_TOKEN_STORAGE_KEY, session.token.trim());
  window.localStorage.setItem(
      BANANA_AUTH_STEAM_ID_STORAGE_KEY, session.steamId.trim());
}

export function clearStoredAuthSession(): void {
  if (typeof window === 'undefined') {
    return;
  }

  window.localStorage.removeItem(BANANA_AUTH_TOKEN_STORAGE_KEY);
  window.localStorage.removeItem(BANANA_AUTH_STEAM_ID_STORAGE_KEY);
}

export function resolveLoginReturnToUrl(): string {
  if (typeof window === 'undefined') {
    return 'http://localhost:5173/login';
  }

  return `${window.location.origin}/login`;
}

export function buildSteamAuthStartUrl(
    apiBaseUrl: string, returnTo: string): string {
  const url = new URL('/auth/steam/start', apiBaseUrl);
  url.searchParams.set('returnTo', returnTo);
  return url.toString();
}

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
