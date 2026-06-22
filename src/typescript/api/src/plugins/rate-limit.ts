import rateLimitPlugin from '@fastify/rate-limit';
import type {FastifyInstance} from 'fastify';

// Per-route config attached via routeOptions.config.rateLimit.
// Bulk endpoints handle up to 128 items per call, so they are given a
// proportionally higher per-minute ceiling than the global default (100).
export const BULK_ROUTE_RATE_LIMIT = {
  max: 600,
  timeWindow: '1 minute',
} as const;

// Routes that receive the bulk uplift — checked during route registration.
export const BULK_RATE_LIMITED_ROUTES = new Set([
  'POST:/api/netcode/k3h4/training-session/confidence/bulk',
  'POST:/api/netcode/k3h4/training-session/geometry/bulk',
]);

export async function registerRateLimitPlugin(app: FastifyInstance):
    Promise<void> {
  await app.register(rateLimitPlugin, {
    global: true,
    max: 100,
    timeWindow: '1 minute',
    // Per-route overrides applied via routeOptions.config.rateLimit
    keyGenerator: (req) => req.headers['x-forwarded-for']?.toString() ?? req.ip,
  });
}
