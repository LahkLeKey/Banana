// Banana Fastify API (spec 008). Owns TypeScript-only routes; ASP.NET (007)
// owns the legacy banana/batch/ripeness/not-banana surface. Per-route
// ownership table:
// .specify/specs/008-typescript-api/contracts/route-ownership.md
import helmet from "@fastify/helmet";
import Fastify from "fastify";

import {registerChatRoutes} from "./domains/chat/routes.ts";
import {registerNotBananaRoutes} from "./domains/not-banana/routes.ts";
import {registerLabelQueueRoutes} from "./domains/training/label-queue.ts";
import {registerTrainingWorkbenchRoutes} from "./domains/training/routes.ts";
import {registerJobRoutes, startJobQueue} from "./jobs/jobs.ts";
import {registerAuditLogPlugin} from "./plugins/audit-log.ts";
import {registerRateLimitPlugin} from "./plugins/rate-limit.ts";
import {registerAuditRoutes} from "./routes/audit.ts";
import {registerCorpusRoutes} from "./routes/corpus.ts";
import {registerDriftRoutes} from "./routes/drift.ts";
import {registerHealthRoutes} from "./routes/health.ts";
import {registryRoutes} from "./routes/registry.ts";
import {registerRipenessRoutes} from "./routes/ripeness.ts";
import {registerStreamingChatRoutes} from "./routes/streaming-chat.ts";
import {registerStreamingRoutes} from "./routes/streaming.ts";
import {webhooksRoutes} from "./routes/webhooks.ts";

const app = Fastify({logger : true});

// Feature 100 — CSP + security headers via @fastify/helmet
await app.register(helmet, {
    contentSecurityPolicy : {
        directives : {
            defaultSrc : [ "'self'" ],
            scriptSrc : [ "'self'" ],
            styleSrc : [ "'self'", "'unsafe-inline'" ],
            imgSrc : [ "'self'", "data:" ],
            connectSrc : [ "'self'" ],
            fontSrc : [ "'self'" ],
            objectSrc : [ "'none'" ],
            upgradeInsecureRequests : [],
        },
    },
    crossOriginEmbedderPolicy : false, // disabled to allow iframe embeds in Electron
});

// Spec #068 — rate limiting (must register before routes)
await registerRateLimitPlugin(app);

// Spec #069 — audit log (hooks onResponse, must register before routes)
await registerAuditLogPlugin(app);

// Spec #126 — BANANA_CORS_ORIGINS: comma-separated list of allowed origins.
// Defaults to localhost dev servers when the env var is absent.
const _corsOriginsEnv =
    process.env.BANANA_CORS_ORIGINS ?? "http://localhost:5173,http://localhost:3000";
const ALLOWED_WEB_ORIGINS =
    new Set(_corsOriginsEnv.split(",").map((o) => o.trim()).filter(Boolean));

app.addHook("onRequest", async (request, reply) => {
    const origin = (request.headers.origin ?? "").toString();
    if (ALLOWED_WEB_ORIGINS.has(origin))
    {
        reply.header("Access-Control-Allow-Origin", origin);
        reply.header("Vary", "Origin");
        reply.header("Access-Control-Allow-Methods", "GET,POST,PUT,PATCH,DELETE,OPTIONS");
        reply.header(
            "Access-Control-Allow-Headers",
            (request.headers["access-control-request-headers"] ?? "content-type").toString());
    }

    if (request.method === "OPTIONS")
    {
        return reply.status(204).send();
    }
});

await registerHealthRoutes(app);
await registerCorpusRoutes(app);
await registerRipenessRoutes(app);
await registerDriftRoutes(app);
await registerNotBananaRoutes(app);
await registerChatRoutes(app);
await registerTrainingWorkbenchRoutes(app);
await registerLabelQueueRoutes(app);
await app.register(registryRoutes);
await app.register(webhooksRoutes);
await registerJobRoutes(app);
await registerAuditRoutes(app);
await registerStreamingRoutes(app);
await registerStreamingChatRoutes(app);

const port = Number(process.env.PORT ?? 8081);
const host = process.env.HOST ?? "0.0.0.0";

// Spec #070 — start durable job queue after server is ready
app.addHook("onReady", async () => {
    startJobQueue().catch((err) => app.log.error({err}, "job queue start failed"));
});

app.listen({port, host}).catch((err) => {
    app.log.error(err);
    process.exit(1);
});
