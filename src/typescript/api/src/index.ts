// Banana Fastify API (spec 008). Owns TypeScript-only routes; ASP.NET (007)
// owns the legacy banana/batch/ripeness/not-banana surface. Per-route
// ownership table:
// .specify/specs/008-typescript-api/contracts/route-ownership.md
import Fastify from 'fastify';

import {registerChatRoutes} from './domains/chat/routes.ts';
import {registerNotBananaRoutes} from './domains/not-banana/routes.ts';
import {registerTrainingWorkbenchRoutes} from './domains/training/routes.ts';
import {registerAuditLogPlugin} from './plugins/audit-log.ts';
import {registerRateLimitPlugin} from './plugins/rate-limit.ts';
import {registerAuditRoutes} from './routes/audit.ts';
import {registerCorpusRoutes} from './routes/corpus.ts';
import {registerHealthRoutes} from './routes/health.ts';
import {registerRipenessRoutes} from './routes/ripeness.ts';
import {registryRoutes} from './routes/registry.ts';
import {registerStreamingRoutes} from './routes/streaming.ts';
import {registerStreamingChatRoutes} from './routes/streaming-chat.ts';

const app = Fastify({logger: true});

// Spec #068 — rate limiting (must register before routes)
await registerRateLimitPlugin(app);

// Spec #069 — audit log (hooks onResponse, must register before routes)
await registerAuditLogPlugin(app);

const ALLOWED_WEB_ORIGINS = new Set([
  'http://localhost:5173',
  'http://127.0.0.1:5173',
]);

app.addHook('onRequest', async (request, reply) => {
  const origin = (request.headers.origin ?? '').toString();
  if (ALLOWED_WEB_ORIGINS.has(origin)) {
    reply.header('Access-Control-Allow-Origin', origin);
    reply.header('Vary', 'Origin');
    reply.header(
        'Access-Control-Allow-Methods', 'GET,POST,PUT,PATCH,DELETE,OPTIONS');
    reply.header(
        'Access-Control-Allow-Headers',
        (request.headers['access-control-request-headers'] ?? 'content-type')
            .toString());
  }

  if (request.method === 'OPTIONS') {
    return reply.status(204).send();
  }
});

await registerHealthRoutes(app);
await registerCorpusRoutes(app);
await registerRipenessRoutes(app);
await registerNotBananaRoutes(app);
await registerChatRoutes(app);
await registerTrainingWorkbenchRoutes(app);
await app.register(registryRoutes);
await registerAuditRoutes(app);
await registerStreamingRoutes(app);
await registerStreamingChatRoutes(app);

const port = Number(process.env.PORT ?? 8081);
const host = process.env.HOST ?? '0.0.0.0';

app.listen({port, host}).catch((err) => {
  app.log.error(err);
  process.exit(1);
});
