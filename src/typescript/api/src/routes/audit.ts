/**
 * audit.ts — Operator-gated audit event read endpoint (spec #069).
 *
 * GET /operator/audit?action=&actor=&limit=50&cursor=
 * Returns { events: AuditEvent[], next_cursor: string | null }
 */

import {PrismaClient} from "@prisma/client";
import type {FastifyInstance} from "fastify";

import {requireRole} from "../middleware/auth.ts";

let prismaClient: PrismaClient|null = null;
let prismaInitAttempted = false;

function getAuditReadPrisma(app: FastifyInstance): PrismaClient|null
{
    if (prismaInitAttempted)
        return prismaClient;
    prismaInitAttempted = true;

    const accelerateUrl = process.env.PRISMA_ACCELERATE_URL;
    if (!accelerateUrl)
    {
        app.log.warn("audit-routes: PRISMA_ACCELERATE_URL not set, endpoint disabled");
        return null;
    }

    try
    {
        prismaClient = new PrismaClient({
            accelerateUrl,
            log : [ "warn", "error" ],
        });
    }
    catch (err)
    {
        app.log.warn({err}, "audit-routes: prisma initialization failed, endpoint disabled");
        prismaClient = null;
    }

    return prismaClient;
}

export async function registerAuditRoutes(app: FastifyInstance): Promise<void>
{
    app.get("/operator/audit", {preHandler : requireRole("operator")}, async (request, reply) => {
        const prisma = getAuditReadPrisma(app);
        if (!prisma)
        {
            return reply.code(503).send({
                error : "audit route unavailable",
                reason : "PRISMA_ACCELERATE_URL is not configured",
            });
        }

        const query = request.query as
        {
            action?: string;
            actor?: string;
            limit?: string;
            cursor?: string;
        };

        const limit = Math.min(Number(query.limit ?? 50), 200);
        const cursor = query.cursor ?? undefined;

        const where: Record<string, unknown> = {};
        if (query.action)
            where.action = {contains : query.action};
        if (query.actor)
            where.actor = query.actor;
        if (cursor)
            where.id = {gt : cursor};

        const events = await prisma.audit_event.findMany({
            where,
            orderBy : {created_at : "desc"},
            take : limit + 1,
        });

        const hasMore = events.length > limit;
        const page = hasMore ? events.slice(0, limit) : events;
        const next_cursor = hasMore ? (page[page.length - 1]?.id ?? null) : null;

        return reply.send({events : page, next_cursor});
    });
}
