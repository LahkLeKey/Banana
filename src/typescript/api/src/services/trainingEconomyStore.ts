import {randomUUID} from 'node:crypto';

import {resolveAuthoritativeDatabaseUrl} from './databaseRuntime.ts';

type PostgresPoolLike = {
  query: (text: string, values?: unknown[]) =>
      Promise<{rowCount?: number; rows: Array<Record<string, unknown>>;}>;
};

export type TrainingQueueStatus = 'queued'|'running'|'completed'|'failed';
export type RewardStatus = 'pending'|'claimed';

export type TrainingJobSeed = {
  title: string; sector: string; rewardXp: number;
};

export type TrainingJobRecord = {
  jobId: string; playerId: string; title: string; sector: string;
  status: TrainingQueueStatus;
  rewardXp: number;
  score: number;
  durationMs: number;
  antiCheatVerdict: 'pending' | 'passed' | 'rejected';
  createdAt: string;
  updatedAt: string;
};

export type RewardRecord = {
  rewardId: string; playerId: string; sourceJobId: string; xp: number;
  status: RewardStatus;
  createdAt: string;
  claimedAt: string | null;
};

export type LeaderboardEntry = {
  playerId: string; totalScore: number; totalXp: number; completedJobs: number;
  updatedAt: string;
};

export type TransitionEventRecord = {
  eventId: string; playerId: string; eventType: string; correlationId: string;
  details: Record<string, unknown>;
  createdAt: string;
};

export type CompleteTrainingJobInput = {
  playerId: string; jobId: string; score: number; durationMs: number;
  integrityProof: string;
};

export type StoreTransitionEventInput = {
  playerId: string; eventType: string; correlationId: string;
  details: Record<string, unknown>;
};

export type TrainingEconomyStore = {
  scaffoldJobs: (playerId: string, seeds: TrainingJobSeed[]) =>
      Promise<TrainingJobRecord[]>;
  listJobs: (playerId: string) => Promise<TrainingJobRecord[]>;
  completeJob: (input: CompleteTrainingJobInput) => Promise<{
    job: TrainingJobRecord; reward: RewardRecord; antiCheatPassed: boolean;
  }>;
  listLeaderboard: (limit: number) => Promise<LeaderboardEntry[]>;
  listRewards: (playerId: string) => Promise<RewardRecord[]>;
  claimReward: (playerId: string, rewardId: string) => Promise<RewardRecord>;
  storeTransitionEvent: (input: StoreTransitionEventInput) =>
      Promise<TransitionEventRecord>;
  listTransitionEvents: (playerId: string, limit: number) =>
      Promise<TransitionEventRecord[]>;
};

function resolveDatabaseUrl(): string {
  return resolveAuthoritativeDatabaseUrl(process.env).url;
}

function shouldUseStrictMode(): boolean {
  return process.env.BANANA_TRAINING_STORE_STRICT === 'true';
}

function shouldDisableSsl(databaseUrl: string): boolean {
  const normalized = databaseUrl.trim().toLowerCase();
  return normalized.includes('localhost') || normalized.includes('127.0.0.1') ||
      normalized.includes('.internal') || normalized.includes('.flycast');
}

function normalizeRewardXp(value: number): number {
  if (!Number.isFinite(value)) {
    return 0;
  }
  return Math.max(10, Math.min(2_500, Math.floor(value)));
}

function normalizeScore(value: number): number {
  if (!Number.isFinite(value)) {
    return 0;
  }
  return Math.max(0, Math.min(100_000, Math.floor(value)));
}

function normalizeDurationMs(value: number): number {
  if (!Number.isFinite(value)) {
    return 0;
  }
  return Math.max(0, Math.min(86_400_000, Math.floor(value)));
}

function nowIso(): string {
  return new Date().toISOString();
}

function antiCheatPasses(
    score: number, durationMs: number, integrityProof: string): boolean {
  if (integrityProof.trim().length < 12) {
    return false;
  }

  if (durationMs < 500 || durationMs > 86_400_000) {
    return false;
  }

  const maxAllowedScore = Math.floor(durationMs / 5) + 5_000;
  return score <= maxAllowedScore;
}

class MemoryTrainingEconomyStore implements TrainingEconomyStore {
  private readonly jobs = new Map<string, TrainingJobRecord>();
  private readonly rewards = new Map<string, RewardRecord>();
  private readonly events: TransitionEventRecord[] = [];

  async scaffoldJobs(playerId: string, seeds: TrainingJobSeed[]):
      Promise<TrainingJobRecord[]> {
    const createdAt = nowIso();
    const records = seeds.map((seed) => {
      const job: TrainingJobRecord = {
        jobId: randomUUID(),
        playerId,
        title: seed.title.trim(),
        sector: seed.sector.trim(),
        status: 'queued',
        rewardXp: normalizeRewardXp(seed.rewardXp),
        score: 0,
        durationMs: 0,
        antiCheatVerdict: 'pending',
        createdAt,
        updatedAt: createdAt,
      };
      this.jobs.set(job.jobId, job);
      return job;
    });

    return records;
  }

  async listJobs(playerId: string): Promise<TrainingJobRecord[]> {
    return Array.from(this.jobs.values())
        .filter((job) => job.playerId === playerId)
        .sort((left, right) => right.updatedAt.localeCompare(left.updatedAt));
  }

  async completeJob(input: CompleteTrainingJobInput): Promise<{
    job: TrainingJobRecord; reward: RewardRecord; antiCheatPassed: boolean;
  }> {
    const job = this.jobs.get(input.jobId);
    if (!job || job.playerId !== input.playerId) {
      throw new Error('training_job_not_found');
    }

    if (job.status === 'completed') {
      const existingReward =
          Array.from(this.rewards.values())
              .find((item) => item.sourceJobId === job.jobId);
      if (!existingReward) {
        throw new Error('training_reward_missing_for_completed_job');
      }
      return {job, reward: existingReward, antiCheatPassed: true};
    }

    const normalizedScore = normalizeScore(input.score);
    const normalizedDuration = normalizeDurationMs(input.durationMs);
    const antiCheatPassed = antiCheatPasses(
        normalizedScore, normalizedDuration, input.integrityProof);

    if (!antiCheatPassed) {
      job.status = 'failed';
      job.score = normalizedScore;
      job.durationMs = normalizedDuration;
      job.antiCheatVerdict = 'rejected';
      job.updatedAt = nowIso();
      throw new Error('training_anti_cheat_rejected');
    }

    job.status = 'completed';
    job.score = normalizedScore;
    job.durationMs = normalizedDuration;
    job.antiCheatVerdict = 'passed';
    job.updatedAt = nowIso();

    const reward: RewardRecord = {
      rewardId: randomUUID(),
      playerId: input.playerId,
      sourceJobId: job.jobId,
      xp: job.rewardXp,
      status: 'pending',
      createdAt: nowIso(),
      claimedAt: null,
    };
    this.rewards.set(reward.rewardId, reward);

    return {job, reward, antiCheatPassed};
  }

  async listLeaderboard(limit: number): Promise<LeaderboardEntry[]> {
    const byPlayer = new Map<string, LeaderboardEntry>();
    for (const job of this.jobs.values()) {
      if (job.status !== 'completed') {
        continue;
      }

      const existing = byPlayer.get(job.playerId) ?? {
        playerId: job.playerId,
        totalScore: 0,
        totalXp: 0,
        completedJobs: 0,
        updatedAt: job.updatedAt,
      };
      existing.totalScore += job.score;
      existing.totalXp += job.rewardXp;
      existing.completedJobs += 1;
      existing.updatedAt = job.updatedAt;
      byPlayer.set(job.playerId, existing);
    }

    return Array.from(byPlayer.values())
        .sort((left, right) => right.totalScore - left.totalScore)
        .slice(0, Math.max(1, Math.min(200, Math.trunc(limit))));
  }

  async listRewards(playerId: string): Promise<RewardRecord[]> {
    return Array.from(this.rewards.values())
        .filter((reward) => reward.playerId === playerId)
        .sort((left, right) => right.createdAt.localeCompare(left.createdAt));
  }

  async claimReward(playerId: string, rewardId: string): Promise<RewardRecord> {
    const reward = this.rewards.get(rewardId);
    if (!reward || reward.playerId !== playerId) {
      throw new Error('reward_not_found');
    }

    if (reward.status === 'claimed') {
      return reward;
    }

    const sourceJob = this.jobs.get(reward.sourceJobId);
    if (!sourceJob || sourceJob.status !== 'completed' ||
        sourceJob.antiCheatVerdict !== 'passed') {
      throw new Error('reward_claim_denied_policy');
    }

    reward.status = 'claimed';
    reward.claimedAt = nowIso();
    return reward;
  }

  async storeTransitionEvent(input: StoreTransitionEventInput):
      Promise<TransitionEventRecord> {
    const event: TransitionEventRecord = {
      eventId: randomUUID(),
      playerId: input.playerId,
      eventType: input.eventType.trim(),
      correlationId: input.correlationId.trim() || randomUUID(),
      details: input.details,
      createdAt: nowIso(),
    };
    this.events.unshift(event);
    if (this.events.length > 3_000) {
      this.events.pop();
    }
    return event;
  }

  async listTransitionEvents(playerId: string, limit: number):
      Promise<TransitionEventRecord[]> {
    const capped = Math.max(1, Math.min(300, Math.trunc(limit)));
    return this.events.filter((event) => event.playerId === playerId)
        .slice(0, capped);
  }
}

class PostgresTrainingEconomyStore implements TrainingEconomyStore {
  constructor(private readonly pool: PostgresPoolLike) {}

  public async init(): Promise<void> {
    await this.pool.query(`
      CREATE TABLE IF NOT EXISTS banana_training_jobs (
        job_id TEXT PRIMARY KEY,
        player_id TEXT NOT NULL,
        title TEXT NOT NULL,
        sector TEXT NOT NULL,
        status TEXT NOT NULL,
        reward_xp INT NOT NULL,
        score INT NOT NULL DEFAULT 0,
        duration_ms INT NOT NULL DEFAULT 0,
        anti_cheat_verdict TEXT NOT NULL,
        created_at TIMESTAMPTZ NOT NULL,
        updated_at TIMESTAMPTZ NOT NULL
      )
    `);

    await this.pool.query(`
      CREATE TABLE IF NOT EXISTS banana_training_rewards (
        reward_id TEXT PRIMARY KEY,
        player_id TEXT NOT NULL,
        source_job_id TEXT NOT NULL REFERENCES banana_training_jobs(job_id) ON DELETE CASCADE,
        xp INT NOT NULL,
        status TEXT NOT NULL,
        created_at TIMESTAMPTZ NOT NULL,
        claimed_at TIMESTAMPTZ NULL
      )
    `);

    await this.pool.query(`
      CREATE TABLE IF NOT EXISTS banana_training_transition_events (
        event_id TEXT PRIMARY KEY,
        player_id TEXT NOT NULL,
        event_type TEXT NOT NULL,
        correlation_id TEXT NOT NULL,
        details_json TEXT NOT NULL,
        created_at TIMESTAMPTZ NOT NULL
      )
    `);

    await this.pool.query(
        'CREATE INDEX IF NOT EXISTS banana_training_jobs_player_idx ON banana_training_jobs (player_id)');
    await this.pool.query(
        'CREATE INDEX IF NOT EXISTS banana_training_rewards_player_idx ON banana_training_rewards (player_id)');
    await this.pool.query(
        'CREATE INDEX IF NOT EXISTS banana_training_events_player_idx ON banana_training_transition_events (player_id, created_at DESC)');
  }

  async scaffoldJobs(playerId: string, seeds: TrainingJobSeed[]):
      Promise<TrainingJobRecord[]> {
    const created: TrainingJobRecord[] = [];
    for (const seed of seeds) {
      const jobId = randomUUID();
      const createdAt = nowIso();
      const record: TrainingJobRecord = {
        jobId,
        playerId,
        title: seed.title.trim(),
        sector: seed.sector.trim(),
        status: 'queued',
        rewardXp: normalizeRewardXp(seed.rewardXp),
        score: 0,
        durationMs: 0,
        antiCheatVerdict: 'pending',
        createdAt,
        updatedAt: createdAt,
      };
      await this.pool.query(
          `
          INSERT INTO banana_training_jobs (
            job_id, player_id, title, sector, status, reward_xp,
            score, duration_ms, anti_cheat_verdict, created_at, updated_at
          ) VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11)
        `,
          [
            record.jobId,
            record.playerId,
            record.title,
            record.sector,
            record.status,
            record.rewardXp,
            record.score,
            record.durationMs,
            record.antiCheatVerdict,
            record.createdAt,
            record.updatedAt,
          ]);
      created.push(record);
    }
    return created;
  }

  async listJobs(playerId: string): Promise<TrainingJobRecord[]> {
    const result = await this.pool.query(
        `
        SELECT *
        FROM banana_training_jobs
        WHERE player_id = $1
        ORDER BY updated_at DESC
      `,
        [playerId]);
    return result.rows.map(
        (row) => ({
          jobId: String(row.job_id),
          playerId: String(row.player_id),
          title: String(row.title),
          sector: String(row.sector),
          status: String(row.status) as TrainingQueueStatus,
          rewardXp: Number(row.reward_xp),
          score: Number(row.score),
          durationMs: Number(row.duration_ms),
          antiCheatVerdict: String(row.anti_cheat_verdict) as 'pending' |
              'passed' | 'rejected',
          createdAt: String(row.created_at),
          updatedAt: String(row.updated_at),
        }));
  }

  async completeJob(input: CompleteTrainingJobInput): Promise<{
    job: TrainingJobRecord; reward: RewardRecord; antiCheatPassed: boolean;
  }> {
    const query = await this.pool.query(
        'SELECT * FROM banana_training_jobs WHERE job_id = $1 AND player_id = $2',
        [input.jobId, input.playerId]);
    const row = query.rows[0];
    if (!row) {
      throw new Error('training_job_not_found');
    }

    const currentStatus = String(row.status) as TrainingQueueStatus;
    if (currentStatus === 'completed') {
      const rewardRows = await this.pool.query(
          'SELECT * FROM banana_training_rewards WHERE source_job_id = $1 LIMIT 1',
          [input.jobId]);
      const reward = rewardRows.rows[0];
      if (!reward) {
        throw new Error('training_reward_missing_for_completed_job');
      }

      return {
        job: {
          jobId: String(row.job_id),
          playerId: String(row.player_id),
          title: String(row.title),
          sector: String(row.sector),
          status: 'completed',
          rewardXp: Number(row.reward_xp),
          score: Number(row.score),
          durationMs: Number(row.duration_ms),
          antiCheatVerdict: 'passed',
          createdAt: String(row.created_at),
          updatedAt: String(row.updated_at),
        },
        reward: {
          rewardId: String(reward.reward_id),
          playerId: String(reward.player_id),
          sourceJobId: String(reward.source_job_id),
          xp: Number(reward.xp),
          status: String(reward.status) as RewardStatus,
          createdAt: String(reward.created_at),
          claimedAt: reward.claimed_at ? String(reward.claimed_at) : null,
        },
        antiCheatPassed: true,
      };
    }

    const score = normalizeScore(input.score);
    const durationMs = normalizeDurationMs(input.durationMs);
    const antiCheatPassed =
        antiCheatPasses(score, durationMs, input.integrityProof);
    if (!antiCheatPassed) {
      await this.pool.query(
          `
          UPDATE banana_training_jobs
          SET status = 'failed', score = $3, duration_ms = $4,
              anti_cheat_verdict = 'rejected', updated_at = $5
          WHERE job_id = $1 AND player_id = $2
        `,
          [input.jobId, input.playerId, score, durationMs, nowIso()]);
      throw new Error('training_anti_cheat_rejected');
    }

    const updatedAt = nowIso();
    await this.pool.query(
        `
        UPDATE banana_training_jobs
        SET status = 'completed', score = $3, duration_ms = $4,
            anti_cheat_verdict = 'passed', updated_at = $5
        WHERE job_id = $1 AND player_id = $2
      `,
        [input.jobId, input.playerId, score, durationMs, updatedAt]);

    const rewardId = randomUUID();
    const createdAt = nowIso();
    await this.pool.query(
        `
        INSERT INTO banana_training_rewards (
          reward_id, player_id, source_job_id, xp, status, created_at, claimed_at
        ) VALUES ($1,$2,$3,$4,'pending',$5,NULL)
      `,
        [
          rewardId, input.playerId, input.jobId, Number(row.reward_xp),
          createdAt
        ]);

    return {
      job: {
        jobId: input.jobId,
        playerId: input.playerId,
        title: String(row.title),
        sector: String(row.sector),
        status: 'completed',
        rewardXp: Number(row.reward_xp),
        score,
        durationMs,
        antiCheatVerdict: 'passed',
        createdAt: String(row.created_at),
        updatedAt,
      },
      reward: {
        rewardId,
        playerId: input.playerId,
        sourceJobId: input.jobId,
        xp: Number(row.reward_xp),
        status: 'pending',
        createdAt,
        claimedAt: null,
      },
      antiCheatPassed: true,
    };
  }

  async listLeaderboard(limit: number): Promise<LeaderboardEntry[]> {
    const capped = Math.max(1, Math.min(200, Math.trunc(limit)));
    const result = await this.pool.query(
        `
        SELECT
          player_id,
          COALESCE(SUM(score), 0) AS total_score,
          COALESCE(SUM(reward_xp), 0) AS total_xp,
          COALESCE(COUNT(*), 0) AS completed_jobs,
          COALESCE(MAX(updated_at), NOW()) AS updated_at
        FROM banana_training_jobs
        WHERE status = 'completed'
        GROUP BY player_id
        ORDER BY total_score DESC
        LIMIT $1
      `,
        [capped]);

    return result.rows.map((row) => ({
                             playerId: String(row.player_id),
                             totalScore: Number(row.total_score),
                             totalXp: Number(row.total_xp),
                             completedJobs: Number(row.completed_jobs),
                             updatedAt: String(row.updated_at),
                           }));
  }

  async listRewards(playerId: string): Promise<RewardRecord[]> {
    const result = await this.pool.query(
        `
        SELECT *
        FROM banana_training_rewards
        WHERE player_id = $1
        ORDER BY created_at DESC
      `,
        [playerId]);
    return result.rows.map(
        (row) => ({
          rewardId: String(row.reward_id),
          playerId: String(row.player_id),
          sourceJobId: String(row.source_job_id),
          xp: Number(row.xp),
          status: String(row.status) as RewardStatus,
          createdAt: String(row.created_at),
          claimedAt: row.claimed_at ? String(row.claimed_at) : null,
        }));
  }

  async claimReward(playerId: string, rewardId: string): Promise<RewardRecord> {
    const rewardRows = await this.pool.query(
        'SELECT * FROM banana_training_rewards WHERE reward_id = $1 AND player_id = $2',
        [rewardId, playerId]);
    const reward = rewardRows.rows[0];
    if (!reward) {
      throw new Error('reward_not_found');
    }

    if (String(reward.status) === 'claimed') {
      return {
        rewardId: String(reward.reward_id),
        playerId: String(reward.player_id),
        sourceJobId: String(reward.source_job_id),
        xp: Number(reward.xp),
        status: 'claimed',
        createdAt: String(reward.created_at),
        claimedAt: reward.claimed_at ? String(reward.claimed_at) : null,
      };
    }

    const jobRows = await this.pool.query(
        'SELECT status, anti_cheat_verdict FROM banana_training_jobs WHERE job_id = $1 LIMIT 1',
        [String(reward.source_job_id)]);
    const job = jobRows.rows[0];
    if (!job || String(job.status) !== 'completed' ||
        String(job.anti_cheat_verdict) !== 'passed') {
      throw new Error('reward_claim_denied_policy');
    }

    const claimedAt = nowIso();
    await this.pool.query(
        `
        UPDATE banana_training_rewards
        SET status = 'claimed', claimed_at = $3
        WHERE reward_id = $1 AND player_id = $2
      `,
        [rewardId, playerId, claimedAt]);

    return {
      rewardId: String(reward.reward_id),
      playerId,
      sourceJobId: String(reward.source_job_id),
      xp: Number(reward.xp),
      status: 'claimed',
      createdAt: String(reward.created_at),
      claimedAt,
    };
  }

  async storeTransitionEvent(input: StoreTransitionEventInput):
      Promise<TransitionEventRecord> {
    const event: TransitionEventRecord = {
      eventId: randomUUID(),
      playerId: input.playerId,
      eventType: input.eventType.trim(),
      correlationId: input.correlationId.trim() || randomUUID(),
      details: input.details,
      createdAt: nowIso(),
    };

    await this.pool.query(
        `
        INSERT INTO banana_training_transition_events (
          event_id, player_id, event_type, correlation_id, details_json, created_at
        ) VALUES ($1,$2,$3,$4,$5,$6)
      `,
        [
          event.eventId,
          event.playerId,
          event.eventType,
          event.correlationId,
          JSON.stringify(event.details ?? {}),
          event.createdAt,
        ]);

    return event;
  }

  async listTransitionEvents(playerId: string, limit: number):
      Promise<TransitionEventRecord[]> {
    const capped = Math.max(1, Math.min(300, Math.trunc(limit)));
    const result = await this.pool.query(
        `
        SELECT *
        FROM banana_training_transition_events
        WHERE player_id = $1
        ORDER BY created_at DESC
        LIMIT $2
      `,
        [playerId, capped]);

    return result.rows.map(
        (row) => ({
          eventId: String(row.event_id),
          playerId: String(row.player_id),
          eventType: String(row.event_type),
          correlationId: String(row.correlation_id),
          details: JSON.parse(String(row.details_json ?? '{}')),
          createdAt: String(row.created_at),
        }));
  }
}

let sharedStorePromise: Promise<TrainingEconomyStore>|null = null;

async function createPostgresPool(databaseUrl: string):
    Promise<PostgresPoolLike> {
  const pg = (await import('pg')) as unknown as {
    Pool: new (config: Record<string, unknown>) => PostgresPoolLike;
  };

  return new pg.Pool({
    connectionString: databaseUrl,
    max: Number(process.env.BANANA_TRAINING_STORE_POOL_SIZE ?? 4),
    ssl: shouldDisableSsl(databaseUrl) ? false : {rejectUnauthorized: false},
    idleTimeoutMillis:
        Number(process.env.BANANA_TRAINING_STORE_IDLE_TIMEOUT_MS ?? 30_000),
    connectionTimeoutMillis:
        Number(process.env.BANANA_TRAINING_STORE_CONNECT_TIMEOUT_MS ?? 8_000),
  });
}

export async function getTrainingEconomyStore(): Promise<TrainingEconomyStore> {
  if (sharedStorePromise) {
    return sharedStorePromise;
  }

  sharedStorePromise = (async () => {
    const databaseUrl = resolveDatabaseUrl();
    if (!databaseUrl) {
      return new MemoryTrainingEconomyStore();
    }

    try {
      const pool = await createPostgresPool(databaseUrl);
      const store = new PostgresTrainingEconomyStore(pool);
      await store.init();
      return store;
    } catch (error) {
      if (shouldUseStrictMode()) {
        throw error;
      }
      return new MemoryTrainingEconomyStore();
    }
  })();

  return sharedStorePromise;
}

export function __resetTrainingEconomyStoreForTests(): void {
  sharedStorePromise = null;
}
