import type { FastifyInstance } from "fastify";

export async function registerEventStoreRoutes(_app: FastifyInstance) {
  // Routes reserved for event store; events are appended via appendEvent().
}

export async function appendEvent(
  app: FastifyInstance,
  type: string,
  model: string | null,
  payload: object,
  actor?: string
): Promise<void> {
  const { prisma } = app as unknown as { prisma: NonNullable<unknown> };
  try {
    // @ts-expect-error — prisma type attached at runtime via plugin
    await prisma.training_event.create({
      data: {
        event_type: type,
        model: model ?? null,
        payload,
        actor: actor ?? null,
      },
    });
  } catch {
    // Event store failures are non-fatal; log and continue.
    app.log.warn({ event_type: type, model }, "training_event append failed");
  }
}

export async function getEvents(
  app: FastifyInstance,
  model?: string,
  limit = 100
): Promise<unknown[]> {
  const { prisma } = app as unknown as { prisma: NonNullable<unknown> };
  // @ts-expect-error — prisma type attached at runtime via plugin
  return prisma.training_event.findMany({
    where: model ? { model } : undefined,
    orderBy: { created_at: "desc" },
    take: limit,
  });
}
