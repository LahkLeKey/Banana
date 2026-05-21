/**
 * registry.ts — Model Registry routes (feature 079).
 *
 * Provides CRUD over ModelRelease and the active-release pointer.
 * All routes are under /registry/* and require the API to have DB configured.
 */
import type { FastifyInstance } from "fastify";
import { z } from "zod";

const ReleaseCreateSchema = z.object({
  modelName: z.enum(["banana", "not-banana", "ripeness"]),
  version: z.string().regex(/^\d+\.\d+\.\d+$/, "must be semver"),
  artifactPath: z.string().min(1),
  artifactHash: z.string().length(64, "must be sha256 hex"),
  manifestJson: z.string().min(2),
  signature: z.string().min(1),
  promotedBy: z.string().min(1),
});

const PromoteSchema = z.object({
  releaseId: z.number().int().positive(),
});

export async function registryRoutes(app: FastifyInstance) {
  const { prisma } = app as unknown as { prisma: NonNullable<unknown> };

  /** List all releases (optionally filtered by modelName) */
  app.get("/registry/releases", async (req, reply) => {
    const { modelName } = req.query as { modelName?: string };
    // @ts-expect-error — prisma type attached at runtime via plugin
    const releases = await prisma.modelRelease.findMany({
      where: modelName ? { modelName } : undefined,
      orderBy: { activeFrom: "desc" },
      take: 100,
    });
    return reply.send(releases);
  });

  /** Create (register) a new model release */
  app.post("/registry/releases", async (req, reply) => {
    const parsed = ReleaseCreateSchema.safeParse(req.body);
    if (!parsed.success) {
      return reply.status(400).send({ error: parsed.error.flatten() });
    }
    const data = parsed.data;
    // @ts-expect-error — prisma type attached at runtime via plugin
    const release = await prisma.modelRelease.create({ data });
    return reply.status(201).send(release);
  });

  /** Get current active release per model */
  app.get("/registry/active", async (_req, reply) => {
    // @ts-expect-error — prisma type attached at runtime via plugin
    const active = await prisma.activeModelRelease.findMany({
      include: { release: true },
    });
    return reply.send(active);
  });

  /** Promote a release to active (upsert ActiveModelRelease) */
  app.post("/registry/promote", async (req, reply) => {
    const parsed = PromoteSchema.safeParse(req.body);
    if (!parsed.success) {
      return reply.status(400).send({ error: parsed.error.flatten() });
    }
    const { releaseId } = parsed.data;
    // @ts-expect-error — prisma type attached at runtime via plugin
    const release = await prisma.modelRelease.findUnique({
      where: { id: BigInt(releaseId) },
    });
    if (!release) return reply.status(404).send({ error: "release not found" });

    // @ts-expect-error — prisma type attached at runtime via plugin
    const active = await prisma.activeModelRelease.upsert({
      where: { modelName: release.modelName },
      create: { modelName: release.modelName, releaseId: release.id },
      update: { releaseId: release.id },
    });
    return reply.send(active);
  });

  /** Rollback: set active release to a specific prior release */
  app.post("/registry/rollback", async (req, reply) => {
    const parsed = PromoteSchema.safeParse(req.body);
    if (!parsed.success) {
      return reply.status(400).send({ error: parsed.error.flatten() });
    }
    const { releaseId } = parsed.data;
    // @ts-expect-error — prisma type attached at runtime via plugin
    const release = await prisma.modelRelease.findUnique({
      where: { id: BigInt(releaseId) },
    });
    if (!release) return reply.status(404).send({ error: "release not found" });

    // @ts-expect-error — prisma type attached at runtime via plugin
    const active = await prisma.activeModelRelease.upsert({
      where: { modelName: release.modelName },
      create: { modelName: release.modelName, releaseId: release.id },
      update: { releaseId: release.id },
    });
    return reply.send(active);
  });
}
