import {randomUUID} from 'node:crypto';

import {resolveAuthoritativeDatabaseUrl} from './databaseRuntime.ts';

export type AuthSessionInsert = {
  steamId: string; tokenHash: string; expiresAt: Date;
  sessionId?: string;
};

export type AuthSessionStore = {
  upsertSteamUser: (steamId: string) => Promise<void>;
  createSession: (input: AuthSessionInsert) => Promise<void>;
  touchActiveSessionByTokenHash: (tokenHash: string, seenAt: Date) =>
      Promise<boolean>;
  revokeSessionByTokenHash: (tokenHash: string, revokedAt: Date) =>
      Promise<boolean>;
};

type PostgresPoolLike = {
  query: (text: string, values?: unknown[]) =>
      Promise<{rowCount?: number; rows: Array<Record<string, unknown>>}>;
  end: () => Promise<void>;
};

const DAY_IN_MS = 24 * 60 * 60 * 1000;

function resolveDatabaseUrl(): string {
  return resolveAuthoritativeDatabaseUrl(process.env).url;
}

function shouldUseStrictMode(): boolean {
  return process.env.BANANA_AUTH_STORE_STRICT === 'true';
}

function shouldDisableSsl(databaseUrl: string): boolean {
  if (process.env.BANANA_AUTH_STORE_SSL_DISABLE === 'true') {
    return true;
  }

  if (databaseUrl.includes('localhost') || databaseUrl.includes('127.0.0.1')) {
    return true;
  }

  return false;
}

class MemoryAuthSessionStore implements AuthSessionStore {
  private readonly users = new Map<string, Date>();
  private readonly sessions = new Map < string, {
    steamId: string;
    expiresAt: Date;
    revokedAt: Date|null;
    seenAt: Date;
  }
  >();

  async upsertSteamUser(steamId: string): Promise<void> {
    this.users.set(steamId, new Date());
  }

  async createSession(input: AuthSessionInsert): Promise<void> {
    this.sessions.set(input.tokenHash, {
      steamId: input.steamId,
      expiresAt: input.expiresAt,
      revokedAt: null,
      seenAt: new Date(),
    });
  }

  async touchActiveSessionByTokenHash(tokenHash: string, seenAt: Date):
      Promise<boolean> {
    const session = this.sessions.get(tokenHash);
    if (!session) {
      return false;
    }

    if (session.revokedAt !== null ||
        session.expiresAt.getTime() <= Date.now()) {
      return false;
    }

    session.seenAt = seenAt;
    return true;
  }

  async revokeSessionByTokenHash(tokenHash: string, revokedAt: Date):
      Promise<boolean> {
    const session = this.sessions.get(tokenHash);
    if (!session) {
      return false;
    }

    session.revokedAt = revokedAt;
    return true;
  }
}

class PostgresAuthSessionStore implements AuthSessionStore {
  constructor(private readonly pool: PostgresPoolLike) {}

  public async init(): Promise<void> {
    await this.pool.query(`
      CREATE TABLE IF NOT EXISTS banana_auth_users (
        steam_id TEXT PRIMARY KEY,
        created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
        updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
        last_login_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
      )
    `);

    await this.pool.query(`
      CREATE TABLE IF NOT EXISTS banana_auth_sessions (
        session_id TEXT PRIMARY KEY,
        steam_id TEXT NOT NULL REFERENCES banana_auth_users(steam_id) ON DELETE CASCADE,
        token_hash TEXT NOT NULL UNIQUE,
        created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
        last_seen_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
        expires_at TIMESTAMPTZ NOT NULL,
        revoked_at TIMESTAMPTZ NULL
      )
    `);

    await this.pool.query(
        `CREATE INDEX IF NOT EXISTS banana_auth_sessions_steam_id_idx ON banana_auth_sessions (steam_id)`);
    await this.pool.query(
        `CREATE INDEX IF NOT EXISTS banana_auth_sessions_token_hash_idx ON banana_auth_sessions (token_hash)`);

    await this.pool.query(
        `DELETE FROM banana_auth_sessions WHERE revoked_at IS NOT NULL OR expires_at < NOW() - INTERVAL '7 days'`);
  }

  async upsertSteamUser(steamId: string): Promise<void> {
    await this.pool.query(
        `
        INSERT INTO banana_auth_users (steam_id)
        VALUES ($1)
        ON CONFLICT (steam_id)
        DO UPDATE SET updated_at = NOW(), last_login_at = NOW()
      `,
        [steamId]);
  }

  async createSession(input: AuthSessionInsert): Promise<void> {
    await this.pool.query(
        `
        INSERT INTO banana_auth_sessions (
          session_id,
          steam_id,
          token_hash,
          expires_at,
          created_at,
          last_seen_at
        ) VALUES ($1, $2, $3, $4, NOW(), NOW())
      `,
        [
          input.sessionId ?? randomUUID(), input.steamId, input.tokenHash,
          input.expiresAt.toISOString()
        ]);
  }

  async touchActiveSessionByTokenHash(tokenHash: string, seenAt: Date):
      Promise<boolean> {
    const result = await this.pool.query(
        `
        UPDATE banana_auth_sessions
        SET last_seen_at = $2
        WHERE token_hash = $1
          AND revoked_at IS NULL
          AND expires_at > NOW()
      `,
        [tokenHash, seenAt.toISOString()]);

    return (result.rowCount ?? 0) > 0;
  }

  async revokeSessionByTokenHash(tokenHash: string, revokedAt: Date):
      Promise<boolean> {
    const result = await this.pool.query(
        `
        UPDATE banana_auth_sessions
        SET revoked_at = $2
        WHERE token_hash = $1
          AND revoked_at IS NULL
      `,
        [tokenHash, revokedAt.toISOString()]);

    return (result.rowCount ?? 0) > 0;
  }
}

let sharedStorePromise: Promise<AuthSessionStore>|null = null;

async function createPostgresPool(databaseUrl: string):
    Promise<PostgresPoolLike> {
  const pg = (await import('pg')) as unknown as {
    Pool: new (config: Record<string, unknown>) => PostgresPoolLike;
  };

  const ssl =
      shouldDisableSsl(databaseUrl) ? false : {rejectUnauthorized: false};
  return new pg.Pool({
    connectionString: databaseUrl,
    max: Number(process.env.BANANA_AUTH_STORE_POOL_SIZE ?? 5),
    ssl,
    idleTimeoutMillis:
        Number(process.env.BANANA_AUTH_STORE_IDLE_TIMEOUT_MS ?? 30_000),
    connectionTimeoutMillis:
        Number(process.env.BANANA_AUTH_STORE_CONNECT_TIMEOUT_MS ?? 8_000),
  });
}

export async function getAuthSessionStore(): Promise<AuthSessionStore> {
  if (sharedStorePromise) {
    return sharedStorePromise;
  }

  sharedStorePromise = (async () => {
    const databaseUrl = resolveDatabaseUrl();
    if (!databaseUrl) {
      return new MemoryAuthSessionStore();
    }

    try {
      const pool = await createPostgresPool(databaseUrl);
      const store = new PostgresAuthSessionStore(pool);
      await store.init();
      return store;
    } catch (error) {
      if (shouldUseStrictMode()) {
        throw error;
      }

      // Keep local/dev auth flow functional when DB is unreachable.
      return new MemoryAuthSessionStore();
    }
  })();

  return sharedStorePromise;
}

export function __resetAuthSessionStoreForTests(): void {
  sharedStorePromise = null;
}

export function deriveDefaultSessionExpiry(now = Date.now()): Date {
  const expiresInMs =
      Number(process.env.BANANA_AUTH_SESSION_TTL_MS ?? 8 * 60 * 60 * 1000);
  const bounded = Number.isFinite(expiresInMs) && expiresInMs > 0 ?
      expiresInMs :
      8 * 60 * 60 * 1000;
  return new Date(now + Math.min(bounded, 30 * DAY_IN_MS));
}
