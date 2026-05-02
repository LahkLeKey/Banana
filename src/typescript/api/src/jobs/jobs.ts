/**
 * jobs.ts — Background job queue (feature 070).
 *
 * Uses pg-boss for durable PostgreSQL-backed job processing.
 * Queue names and worker registration are centralized here.
 *
 * Workers are registered at startup and process jobs asynchronously.
 * Queue is exported for use from route handlers and other domain code.
 */
import PgBoss from "pg-boss";

const DATABASE_URL = process.env.DATABASE_URL ?? "postgresql://banana:banana@localhost:5432/banana";

let boss: PgBoss|null = null;

/** Initialize and start the job queue. Call once at server startup. */
export async function startJobQueue(): Promise<PgBoss>
{
    if (boss)
        return boss;
    boss = new PgBoss({connectionString : DATABASE_URL});

    boss.on("error", (err) => console.error("[pg-boss] error", err));

    await boss.start();
    await registerWorkers(boss);
    return boss;
}

/** Stop the job queue gracefully. */
export async function stopJobQueue(): Promise<void>
{
    if (!boss)
        return;
    await boss.stop();
    boss = null;
}

/** Get the running boss instance (throws if not started). */
export function getQueue(): PgBoss
{
    if (!boss)
        throw new Error("Job queue not started. Call startJobQueue() first.");
    return boss;
}

// ────────────────────────────────────────────────────────────────────────────
// Queue names (feature 070 canonical list)
// ────────────────────────────────────────────────────────────────────────────
export const QUEUES = {
    MODEL_WARMUP : "model-warmup",
    CORPUS_INDEX_REBUILD : "corpus-index-rebuild",
    TRAINING_SESSION_KICKOFF : "training-session-kickoff",
} as const;

// ────────────────────────────────────────────────────────────────────────────
// Worker registration
// ────────────────────────────────────────────────────────────────────────────
async function registerWorkers(b: PgBoss): Promise<void>
{
    await b.createQueue(QUEUES.MODEL_WARMUP);
    await b.createQueue(QUEUES.CORPUS_INDEX_REBUILD);
    await b.createQueue(QUEUES.TRAINING_SESSION_KICKOFF);

    b.work(QUEUES.MODEL_WARMUP, {teamSize : 1, teamConcurrency : 1}, async (job) => {
        console.info("[jobs] model-warmup", job.data);
        // TODO: invoke model warmup logic (feature 079 registry integration)
    });

    b.work(QUEUES.CORPUS_INDEX_REBUILD, {teamSize : 1, teamConcurrency : 1}, async (job) => {
        console.info("[jobs] corpus-index-rebuild", job.data);
        // TODO: trigger corpus index rebuild
    });

    b.work(QUEUES.TRAINING_SESSION_KICKOFF, {teamSize : 1, teamConcurrency : 1}, async (job) => {
        console.info("[jobs] training-session-kickoff", job.data);
        // TODO: spawn training session via classifier script
    });
}

// ────────────────────────────────────────────────────────────────────────────
// Fastify plugin to expose job queue on app instance
// ────────────────────────────────────────────────────────────────────────────
import type {FastifyInstance} from "fastify";

export async function registerJobRoutes(app: FastifyInstance)
{
    /** Enqueue a model warmup job (admin-only in production via requireRole) */
    app.post("/jobs/model-warmup", async (req, reply) => {
        const queue = getQueue();
        const id = await queue.send(QUEUES.MODEL_WARMUP, req.body ?? {});
        return reply.status(202).send({jobId : id});
    });

    /** Enqueue a corpus index rebuild */
    app.post("/jobs/corpus-index-rebuild", async (req, reply) => {
        const queue = getQueue();
        const id = await queue.send(QUEUES.CORPUS_INDEX_REBUILD, req.body ?? {});
        return reply.status(202).send({jobId : id});
    });
}
