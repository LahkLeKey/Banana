export type AppRuntimeConfig = {
  host: string; gatewayPort: number; bananaPort: number; batchPort: number;
  ripenessPort: number;
  mlPort: number;
  legacyApiBaseUrl: string;
  legacyMlApiBaseUrl: string;
  bananaDomainMode: 'auto' | 'native' | 'proxy';
};

function parseBananaDomainMode(value: string|undefined): 'auto'|'native'|
    'proxy' {
  if (!value) {
    return 'auto';
  }

  const normalized = value.trim().toLowerCase();
  if (normalized === 'native' || normalized === 'proxy' ||
      normalized === 'auto') {
    return normalized;
  }

  return 'auto';
}

function parsePort(value: string|undefined, fallback: number): number {
  if (!value) {
    return fallback;
  }

  const parsed = Number(value);
  if (!Number.isInteger(parsed) || parsed <= 0) {
    return fallback;
  }

  return parsed;
}

export function loadRuntimeConfig(env: NodeJS.ProcessEnv = process.env):
    AppRuntimeConfig {
  const legacy =
      env.BANANA_LEGACY_API_BASE_URL?.trim() || 'http://localhost:8080';

  return {
    host: env.BANANA_API_HOST?.trim() || '0.0.0.0',
    gatewayPort: parsePort(env.BANANA_API_PORT, 8180),
    bananaPort: parsePort(env.BANANA_BANANA_API_PORT, 8181),
    batchPort: parsePort(env.BANANA_BATCH_API_PORT, 8182),
    ripenessPort: parsePort(env.BANANA_RIPENESS_API_PORT, 8183),
    mlPort: parsePort(env.BANANA_ML_API_PORT, 8184),
    legacyApiBaseUrl: legacy,
    legacyMlApiBaseUrl: env.BANANA_LEGACY_ML_API_BASE_URL?.trim() || legacy,
    bananaDomainMode: parseBananaDomainMode(env.BANANA_BANANA_DOMAIN_MODE),
  };
}
