import type {FastifyInstance} from 'fastify';
import {randomUUID} from 'node:crypto';
import {appendFile, mkdir} from 'node:fs/promises';
import {dirname} from 'node:path';
import {z} from 'zod';

// Per spec 008 route-ownership: TypeScript-only surface lives here.
const FeedbackEntry = z.object({
  text: z.string().min(1),
  label: z.enum(['banana', 'not-banana']),
  source: z.string().default('user'),
});

const FeedbackQuery = z.object({
  limit: z.coerce.number().int().min(1).max(200).default(50),
});

type StoredFeedback = {
  id: string; text: string; label: 'banana' | 'not-banana'; source: string;
  created_at: string;
  persistence: 'memory' | 'jsonl-audit';
};

type PersistenceResult = {
  mode: 'memory'|'jsonl-audit'; audit_written: boolean;
  warning?: string;
};

const feedbackEntries: StoredFeedback[] = [];
const MAX_FEEDBACK_ENTRIES = 500;

function getCorpusAuditPath(): string {
  return process.env.BANANA_CORPUS_AUDIT_PATH ??
      'artifacts/corpus-feedback.jsonl';
}

async function writeAuditEntry(
    requestJson: string, responseJson: string): Promise<PersistenceResult> {
  if (process.env.BANANA_CORPUS_AUDIT_DISABLED === '1') {
    return {
      mode: 'memory',
      audit_written: false,
      warning: 'corpus_audit_disabled',
    };
  }

  try {
    const auditPath = getCorpusAuditPath();
    const record = JSON.stringify({
      domain: 'corpus',
      route: '/corpus/feedback',
      statusCode: 202,
      requestJson,
      responseJson,
      createdAt: new Date().toISOString(),
    });

    await mkdir(dirname(auditPath), {recursive: true});
    await appendFile(auditPath, `${record}\n`, 'utf8');

    return {
      mode: 'jsonl-audit',
      audit_written: true,
    };
  } catch (error) {
    return {
      mode: 'memory',
      audit_written: false,
      warning: `jsonl_audit_write_failed:${
          error instanceof Error ? error.message : 'unknown'}`,
    };
  }
}

function rememberFeedback(entry: StoredFeedback) {
  feedbackEntries.push(entry);
  if (feedbackEntries.length > MAX_FEEDBACK_ENTRIES) feedbackEntries.shift();
}

export function __resetCorpusRouteStateForTests() {
  feedbackEntries.length = 0;
}

export async function registerCorpusRoutes(app: FastifyInstance) {
  app.post('/corpus/feedback', async (req, reply) => {
    const parsed = FeedbackEntry.safeParse(req.body);
    if (!parsed.success)
      return reply.status(400).send(
          {error: 'invalid_argument', issues: parsed.error.flatten()});

    const entry: StoredFeedback = {
      id: `feedback_${randomUUID()}`,
      text: parsed.data.text,
      label: parsed.data.label,
      source: parsed.data.source,
      created_at: new Date().toISOString(),
      persistence: 'memory',
    };

    const responsePayload = {
      accepted: {
        id: entry.id,
        text: entry.text,
        label: entry.label,
        source: entry.source,
        created_at: entry.created_at,
      },
    };

    const persistence = await writeAuditEntry(
        JSON.stringify(parsed.data), JSON.stringify(responsePayload));
    entry.persistence = persistence.mode;
    rememberFeedback(entry);

    return reply.status(202).send({
      ...responsePayload,
      persistence,
    });
  });

  app.get('/corpus/feedback', async (req, reply) => {
    const parsed = FeedbackQuery.safeParse(req.query ?? {});
    if (!parsed.success)
      return reply.status(400).send(
          {error: 'invalid_argument', issues: parsed.error.flatten()});

    const rows = feedbackEntries.slice(-parsed.data.limit).reverse();
    return reply.status(200).send({
      count: rows.length,
      rows,
    });
  });
}
