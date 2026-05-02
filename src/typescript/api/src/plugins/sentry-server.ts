import type { FastifyInstance } from "fastify";

// Server-side Sentry bootstrap for Fastify.
// DSN from SENTRY_DSN env var; disabled when absent.
export async function registerSentryPlugin(app: FastifyInstance): Promise<void> {
  const dsn = process.env.SENTRY_DSN;
  if (!dsn) return;
  const { init, captureException } = await import("@sentry/node");
  init({ dsn, tracesSampleRate: 0.1 });
  app.addHook("onError", async (_req, _reply, error) => {
    captureException(error);
  });
}
