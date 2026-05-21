import type { FastifyInstance } from "fastify";
import { getDriftStats } from "../plugins/drift-detection.ts";

const ALERT_THRESHOLD = 0.2;

export async function registerDriftRoutes(app: FastifyInstance) {
  app.get("/operator/drift", async (_req, reply) => {
    const stats = getDriftStats();
    const alert =
      stats.psi !== null && stats.psi > ALERT_THRESHOLD
        ? { triggered: true, reason: `PSI ${stats.psi.toFixed(3)} > threshold ${ALERT_THRESHOLD}` }
        : { triggered: false };
    return reply.status(200).send({
      ...stats,
      alertThreshold: ALERT_THRESHOLD,
      alert,
    });
  });
}
