import type { FastifyInstance } from "fastify";
import { z } from "zod";

const EnqueueBody = z.object({
  text: z.string().min(1),
  model: z.enum(["banana", "not-banana", "ripeness"]),
  current_prediction: z.record(z.unknown()).optional(),
  uncertainty: z.number().min(0).max(1).optional(),
});

const ListQuery = z.object({
  status: z.enum(["queued", "labelled", "skipped"]).optional(),
  model: z.enum(["banana", "not-banana", "ripeness"]).optional(),
  limit: z.coerce.number().int().min(1).max(100).default(20),
});

const IdParams = z.object({
  id: z.string().min(1),
});

const LabelBody = z.object({
  label: z.string().min(1),
  labelled_by: z.string().min(1).optional(),
});

export async function registerLabelQueueRoutes(app: FastifyInstance) {
  const { prisma } = app as unknown as { prisma: NonNullable<unknown> };

  app.post("/operator/label-queue", async (req, reply) => {
    const parsed = EnqueueBody.safeParse(req.body ?? {});
    if (!parsed.success) {
      return reply.status(400).send({ error: "invalid_argument", issues: parsed.error.flatten() });
    }
    const { text, model, current_prediction, uncertainty } = parsed.data;
    // @ts-expect-error — prisma type attached at runtime via plugin
    const record = await prisma.label_request.create({
      data: {
        text,
        model,
        current_prediction: current_prediction ?? null,
        uncertainty: uncertainty ?? null,
      },
    });
    return reply.status(201).send({ label_request: record });
  });

  app.get("/operator/label-queue", async (req, reply) => {
    const parsed = ListQuery.safeParse(req.query ?? {});
    if (!parsed.success) {
      return reply.status(400).send({ error: "invalid_argument", issues: parsed.error.flatten() });
    }
    const { status, model, limit } = parsed.data;
    // @ts-expect-error — prisma type attached at runtime via plugin
    const rows = await prisma.label_request.findMany({
      where: {
        ...(status ? { status } : {}),
        ...(model ? { model } : {}),
      },
      orderBy: { created_at: "asc" },
      take: limit,
    });
    return reply.status(200).send({ count: rows.length, rows });
  });

  app.patch("/operator/label-queue/:id", async (req, reply) => {
    const params = IdParams.safeParse(req.params);
    const body = LabelBody.safeParse(req.body ?? {});
    if (!params.success || !body.success) {
      return reply.status(400).send({
        error: "invalid_argument",
        issues: {
          params: params.success ? undefined : params.error.flatten(),
          body: body.success ? undefined : body.error.flatten(),
        },
      });
    }
    // @ts-expect-error — prisma type attached at runtime via plugin
    const existing = await prisma.label_request.findUnique({ where: { id: params.data.id } });
    if (!existing) {
      return reply.status(404).send({ error: "not_found" });
    }
    // @ts-expect-error — prisma type attached at runtime via plugin
    const updated = await prisma.label_request.update({
      where: { id: params.data.id },
      data: {
        label: body.data.label,
        labelled_by: body.data.labelled_by ?? null,
        labelled_at: new Date(),
        status: "labelled",
      },
    });
    return reply.status(200).send({ label_request: updated });
  });
}
