import type { FastifyInstance } from "fastify";

export async function registerHealthRoutes(app: FastifyInstance) {
  app.get("/health", async () => ({ status: "ok" }));
  app.get("/ready", async () => {
    const dbConfigured = Boolean(process.env.DATABASE_URL);
    return { status: dbConfigured ? "ready" : "config_missing" };
  });
}
