// Banana Fastify API (spec 008). Owns TypeScript-only routes; ASP.NET (007)
// owns the legacy banana/batch/ripeness/not-banana surface. Per-route
// ownership table:
// .specify/specs/008-typescript-api/contracts/route-ownership.md
import helmet from '@fastify/helmet';
import Fastify from 'fastify';

import {registerChatRoutes} from './domains/chat/routes.ts';
import {registerNotBananaRoutes} from './domains/not-banana/routes.ts';
import {registerTrainingWorkbenchRoutes} from './domains/training/routes.ts';
import {registerJobRoutes, startJobQueue} from './jobs/jobs.ts';
import {registerCorpusRoutes} from './routes/corpus.ts';
import {registerHealthRoutes} from './routes/health.ts';
import {registerRipenessRoutes} from './routes/ripeness.ts';
import {registryRoutes} from './routes/registry.ts';

const app = Fastify({logger : true});

// Feature 100 — CSP + security headers via @fastify/helmet
await app.register(helmet, {
    contentSecurityPolicy : {
        directives : {
            defaultSrc : [ "'self'" ],
            scriptSrc : [ "'self'" ],
            styleSrc : [ "'self'", "'unsafe-inline'" ],
            imgSrc : [ "'self'", 'data:' ],
            connectSrc : [ "'self'" ],
            fontSrc : [ "'self'" ],
            objectSrc : [ "'none'" ],
            upgradeInsecureRequests : [],
        },
    },
    crossOriginEmbedderPolicy : false, // disabled to allow iframe embeds in Electron
});

const ALLOWED_WEB_ORIGINS = new Set([
    'http://localhost:5173',
    'http://127.0.0.1:5173',
]);

app.addHook('onRequest', async (request, reply) => {
    const origin = (request.headers.origin ?? '').toString();
    if (ALLOWED_WEB_ORIGINS.has(origin))
    {
        reply.header('Access-Control-Allow-Origin', origin);
        reply.header('Vary', 'Origin');
        reply.header('Access-Control-Allow-Methods', 'GET,POST,PUT,PATCH,DELETE,OPTIONS');
        reply.header(
            'Access-Control-Allow-Headers',
            (request.headers['access-control-request-headers'] ?? 'content-type').toString());
    }

    if (request.method === 'OPTIONS')
    {
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

const port = Number(process.env.PORT ?? 8081);
const host = process.env.HOST ?? '0.0.0.0';

app.listen({port, host}).catch((err) => {
    app.log.error(err);
    process.exit(1);
});
