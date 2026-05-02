import rateLimitPlugin from "@fastify/rate-limit";
import type { FastifyInstance } from "fastify";

export async function registerRateLimitPlugin(app: FastifyInstance): Promise<void> {
  await app.register(rateLimitPlugin, {
    global: true,
    max: 100,
    timeWindow: "1 minute",
    // Per-route overrides applied via routeOptions.config.rateLimit
    keyGenerator: (req) => req.headers["x-forwarded-for"]?.toString() ?? req.ip,
  });
}
