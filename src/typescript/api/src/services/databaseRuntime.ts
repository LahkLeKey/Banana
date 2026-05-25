import {createDefaultNativePgBouncerBridge, type NativePgBouncerBridge,} from './nativePgBouncer.ts';

export type DatabaseUrlSource =
    |'NEON_DATABASE_URL'|'DATABASE_URL'|'BANANA_PG_CONNECTION'|'none';

export type NativePgBouncerRuntimeStatus = {
  attempted: boolean; available: boolean; configured: boolean;
  reason?: string;
  poolMode?: string;
  defaultPoolSize?: number;
};

export type DatabaseRuntimeStatus = {
  authoritativeUrlPresent: boolean; source: DatabaseUrlSource;
  aliasesSynced: boolean;
  persistentWorldSourceOfRecordReady: boolean;
  nativePgBouncer: NativePgBouncerRuntimeStatus;
};

export type DatabaseRuntimeOptions = {
  env?: NodeJS.ProcessEnv;
  bridgeFactory?: () => NativePgBouncerBridge;
};

const DEFAULT_NATIVE_POOL_MODE = 'transaction';
const DEFAULT_NATIVE_POOL_SIZE = 20;

let lastRuntimeStatus: DatabaseRuntimeStatus = {
  authoritativeUrlPresent: false,
  source: 'none',
  aliasesSynced: false,
  persistentWorldSourceOfRecordReady: false,
  nativePgBouncer: {
    attempted: false,
    available: false,
    configured: false,
    reason: 'not_bootstrapped',
  },
};

function trim(value: string|undefined): string {
  return (value ?? '').trim();
}

export function resolveAuthoritativeDatabaseUrl(
    env: NodeJS.ProcessEnv =
        process.env): {url: string; source: DatabaseUrlSource;} {
  const neon = trim(env.NEON_DATABASE_URL);
  if (neon) {
    return {url: neon, source: 'NEON_DATABASE_URL'};
  }

  const databaseUrl = trim(env.DATABASE_URL);
  if (databaseUrl) {
    return {url: databaseUrl, source: 'DATABASE_URL'};
  }

  const bananaPg = trim(env.BANANA_PG_CONNECTION);
  if (bananaPg) {
    return {url: bananaPg, source: 'BANANA_PG_CONNECTION'};
  }

  return {url: '', source: 'none'};
}

export function syncDatabaseUrlAliases(env: NodeJS.ProcessEnv = process.env): {
  authoritativeUrl: string; source: DatabaseUrlSource; aliasesSynced: boolean;
} {
  const {url, source} = resolveAuthoritativeDatabaseUrl(env);
  if (!url) {
    return {authoritativeUrl: '', source, aliasesSynced: false};
  }

  env.NEON_DATABASE_URL = url;
  env.DATABASE_URL = url;
  env.BANANA_PG_CONNECTION = url;

  return {authoritativeUrl: url, source, aliasesSynced: true};
}

function shouldConfigureNativePgBouncer(env: NodeJS.ProcessEnv): boolean {
  return trim(env.BANANA_NATIVE_PGBOUNCER_ENABLED) !== 'false';
}

function shouldUseStrictBootstrap(env: NodeJS.ProcessEnv): boolean {
  return trim(env.BANANA_NEON_STRICT) === 'true';
}

function shouldRequirePersistentWorldSourceOfRecord(env: NodeJS.ProcessEnv):
    boolean {
  return trim(env.BANANA_PERSISTENT_WORLD_REQUIRE_SOURCE_OF_RECORD) === 'true';
}

function resolveConfiguredAliases(env: NodeJS.ProcessEnv): string[] {
  return [
    trim(env.NEON_DATABASE_URL),
    trim(env.DATABASE_URL),
    trim(env.BANANA_PG_CONNECTION),
  ].filter(Boolean);
}

function hasConflictingAliases(env: NodeJS.ProcessEnv): boolean {
  const configured = resolveConfiguredAliases(env);
  if (configured.length <= 1) {
    return false;
  }
  const first = configured[0];
  return configured.some((value) => value !== first);
}

function resolveNativePoolMode(env: NodeJS.ProcessEnv): string {
  return trim(env.BANANA_NATIVE_PGBOUNCER_MODE) || DEFAULT_NATIVE_POOL_MODE;
}

function resolveNativePoolSize(env: NodeJS.ProcessEnv): number {
  const parsed = Number(trim(env.BANANA_NATIVE_PGBOUNCER_POOL_SIZE));
  if (!Number.isFinite(parsed) || parsed <= 0) {
    return DEFAULT_NATIVE_POOL_SIZE;
  }
  return Math.floor(parsed);
}

export function bootstrapDatabaseRuntime(
    options: DatabaseRuntimeOptions = {},
    ): DatabaseRuntimeStatus {
  const env = options.env ?? process.env;
  const bridgeFactory =
      options.bridgeFactory ?? createDefaultNativePgBouncerBridge;

  if (hasConflictingAliases(env)) {
    throw new Error(
        'Conflicting source-of-record aliases detected: NEON_DATABASE_URL, DATABASE_URL, and BANANA_PG_CONNECTION must match.');
  }

  const aliasResult = syncDatabaseUrlAliases(env);
  if (!aliasResult.authoritativeUrl && shouldUseStrictBootstrap(env)) {
    throw new Error(
        'No authoritative database URL found while BANANA_NEON_STRICT=true.');
  }
  const status: DatabaseRuntimeStatus = {
    authoritativeUrlPresent: aliasResult.authoritativeUrl.length > 0,
    source: aliasResult.source,
    aliasesSynced: aliasResult.aliasesSynced,
    persistentWorldSourceOfRecordReady:
        aliasResult.authoritativeUrl.length > 0 && aliasResult.aliasesSynced,
    nativePgBouncer: {
      attempted: false,
      available: false,
      configured: false,
      reason: 'disabled_or_missing_url',
    },
  };

  if (!status.authoritativeUrlPresent || !shouldConfigureNativePgBouncer(env)) {
    ensurePersistentWorldSourceOfRecordReady(status, env);
    lastRuntimeStatus = status;
    return status;
  }

  status.nativePgBouncer.attempted = true;

  try {
    const bridge = bridgeFactory();
    status.nativePgBouncer.available = bridge.isAvailable();

    if (!status.nativePgBouncer.available) {
      status.nativePgBouncer.reason = 'native_bridge_unavailable';
      if (shouldUseStrictBootstrap(env)) {
        throw new Error(
            'Native PgBouncer bridge unavailable while BANANA_NEON_STRICT=true.');
      }
      ensurePersistentWorldSourceOfRecordReady(status, env);
      lastRuntimeStatus = status;
      return status;
    }

    bridge.configure({
      connectionUri: aliasResult.authoritativeUrl,
      poolMode: resolveNativePoolMode(env),
      defaultPoolSize: resolveNativePoolSize(env),
    });

    const health = bridge.health();
    status.nativePgBouncer.configured = health.enabled;
    status.nativePgBouncer.reason = health.reason;
    status.nativePgBouncer.poolMode = health.poolMode;
    status.nativePgBouncer.defaultPoolSize = health.defaultPoolSize;

    if (!status.nativePgBouncer.configured && shouldUseStrictBootstrap(env)) {
      throw new Error(
          'Native PgBouncer bridge did not report enabled state while BANANA_NEON_STRICT=true.');
    }

    ensurePersistentWorldSourceOfRecordReady(status, env);
    lastRuntimeStatus = status;
    return status;
  } catch (error) {
    status.nativePgBouncer.reason = String(error);
    if (shouldUseStrictBootstrap(env)) {
      throw error;
    }
    ensurePersistentWorldSourceOfRecordReady(status, env);
    lastRuntimeStatus = status;
    return status;
  }
}

export function ensurePersistentWorldSourceOfRecordReady(
    status: DatabaseRuntimeStatus,
    env: NodeJS.ProcessEnv = process.env,
    ): void {
  if (!shouldRequirePersistentWorldSourceOfRecord(env)) {
    return;
  }

  if (!status.persistentWorldSourceOfRecordReady) {
    throw new Error(
        'persistent_world_source_of_record_unavailable: configure one authoritative database url and keep aliases in sync');
  }
}

export function getDatabaseRuntimeStatus(): DatabaseRuntimeStatus {
  return lastRuntimeStatus;
}
