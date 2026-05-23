// Banana Fastify API (spec 008). Canonical API runtime surface.
// Per-route ownership table:
// .specify/specs/008-typescript-api/contracts/route-ownership.md
import helmet from '@fastify/helmet';
import Fastify from 'fastify';

import {registerChatRoutes} from './domains/chat/routes.ts';
import {registerRateLimitPlugin} from './plugins/rate-limit.ts';
import {registerAuthRoutes} from './routes/auth.ts';
import {registerGameSessionRoutes} from './routes/game-session.ts';
import {registerHealthRoutes} from './routes/health.ts';
import {registerWorldRoutes} from './routes/world.ts';

const app = Fastify({logger: true});

// Feature 100 — CSP + security headers via @fastify/helmet
await app.register(helmet, {
  contentSecurityPolicy: {
    directives: {
      defaultSrc: ['\'self\''],
      scriptSrc: ['\'self\''],
      styleSrc: ['\'self\'', '\'unsafe-inline\''],
      imgSrc: ['\'self\'', 'data:'],
      connectSrc: ['\'self\''],
      fontSrc: ['\'self\''],
      objectSrc: ['\'none\''],
      upgradeInsecureRequests: [],
    },
  },
  crossOriginEmbedderPolicy:
      false,  // disabled to allow iframe embeds in Electron
});

// Spec #068 — rate limiting (must register before routes)
await registerRateLimitPlugin(app);

// Spec #126 — BANANA_CORS_ORIGINS: comma-separated list of allowed origins.
// Defaults to localhost dev servers when the env var is absent.
const _corsOriginsEnv = process.env.BANANA_CORS_ORIGINS ??
    'http://localhost:5173,http://127.0.0.1:5173,http://localhost:5174,http://127.0.0.1:5174,http://localhost:5175,http://127.0.0.1:5175,http://localhost:5176,http://127.0.0.1:5176,http://localhost:3000,http://127.0.0.1:3000';
const ALLOWED_WEB_ORIGINS =
    new Set(_corsOriginsEnv.split(',').map((o) => o.trim()).filter(Boolean));

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
await registerAuthRoutes(app);
await registerGameSessionRoutes(app);
await registerChatRoutes(app);
await registerWorldRoutes(app);

const port = Number(process.env.PORT ?? 8081);
const host = process.env.HOST ?? '0.0.0.0';

app.listen({port, host}).catch((err) => {
  app.log.error(err);
  process.exit(1);
});
