function sanitizeAppPath(
    rawValue: string|null|undefined, fallback = '/'): string {
  const value = (rawValue ?? '').trim();
  if (!value) {
    return fallback;
  }

  if (value.startsWith('/')) {
    return value;
  }

  if (typeof window === 'undefined') {
    return fallback;
  }

  try {
    const parsed = new URL(value, window.location.origin);
    if (parsed.origin === window.location.origin) {
      return `${parsed.pathname}${parsed.search}${parsed.hash}`;
    }
  } catch {
    return fallback;
  }

  return fallback;
}

export function buildProtectedLoginPath(returnToPath: string): string {
  const safeReturnTo = sanitizeAppPath(returnToPath, '/');
  if (safeReturnTo === '/') {
    return '/login';
  }

  const searchParams = new URLSearchParams();
  searchParams.set('returnTo', safeReturnTo);
  return `/login?${searchParams.toString()}`;
}

export function resolveLoginReturnTarget(search: string): string {
  const searchParams = new URLSearchParams(search);
  return sanitizeAppPath(searchParams.get('returnTo'), '/');
}

export function buildLoginStartUrl(returnToPath: string): string {
  const loginUrl = new URL('/login', window.location.origin);
  const safeReturnTo = sanitizeAppPath(returnToPath, '/');
  if (safeReturnTo !== '/') {
    loginUrl.searchParams.set('returnTo', safeReturnTo);
  }

  return loginUrl.toString();
}
