type ParsedOriginPattern = {
  protocol: string; host: string; port: string;
  wildcardHostSuffix: string | null;
};

function parseOriginPattern(pattern: string): ParsedOriginPattern|null {
  try {
    const parsed = new URL(pattern);
    if (parsed.pathname !== '/' || parsed.search.length > 0 ||
        parsed.hash.length > 0) {
      return null;
    }

    const host = parsed.hostname.trim().toLowerCase();
    if (host.length === 0) {
      return null;
    }

    const wildcardHostSuffix = host.startsWith('*.') ? host.slice(1) : null;
    return {
      protocol: parsed.protocol,
      host,
      port: parsed.port,
      wildcardHostSuffix,
    };
  } catch {
    return null;
  }
}

function matchesOriginPattern(
    origin: URL,
    pattern: ParsedOriginPattern,
    ): boolean {
  if (origin.protocol !== pattern.protocol) {
    return false;
  }

  if (origin.port !== pattern.port) {
    return false;
  }

  const normalizedOriginHost = origin.hostname.trim().toLowerCase();
  if (pattern.wildcardHostSuffix) {
    return normalizedOriginHost.endsWith(pattern.wildcardHostSuffix) &&
        normalizedOriginHost.length > pattern.wildcardHostSuffix.length;
  }

  return normalizedOriginHost === pattern.host;
}

export function buildCorsOriginMatcher(originsEnv: string): (origin: string) =>
    boolean {
  const patterns =
      originsEnv.split(',')
          .map((entry) => entry.trim())
          .filter(Boolean)
          .map(parseOriginPattern)
          .filter(
              (pattern): pattern is ParsedOriginPattern => pattern !== null);

  return (origin: string) => {
    const normalizedOrigin = origin.trim();
    if (normalizedOrigin.length === 0) {
      return false;
    }

    try {
      const parsedOrigin = new URL(normalizedOrigin);
      if (parsedOrigin.pathname !== '/' || parsedOrigin.search.length > 0 ||
          parsedOrigin.hash.length > 0) {
        return false;
      }

      return patterns.some(
          (pattern) => matchesOriginPattern(parsedOrigin, pattern));
    } catch {
      return false;
    }
  };
}
