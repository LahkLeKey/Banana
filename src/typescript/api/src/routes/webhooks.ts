/**
 * webhooks.ts — Outbound Webhook Delivery System (spec #071).
 *
 * Subscribers register a URL + set of event names. When the API emits an event,
 * all active matching subscribers receive an HMAC-SHA256-signed POST.
 *
 * Routes:
 *   POST   /webhooks/subscribers          — Register a subscriber
 *   GET    /webhooks/subscribers          — List subscribers
 *   DELETE /webhooks/subscribers/:id      — Remove a subscriber
 *   GET    /webhooks/deliveries           — List delivery attempts
 *
 * Internal:
 *   deliverWebhookEvent(prisma, event, payload) — call from business logic
 */
import type {FastifyInstance} from "fastify";
import {createHmac, randomBytes, randomUUID} from "node:crypto";
import {z} from "zod";

// ---------------------------------------------------------------------------
// HMAC helpers
// ---------------------------------------------------------------------------

/** Compute HMAC-SHA256 signature for a payload string. */
export function signWebhookPayload(secret: string, payload: string): string
{
    return createHmac("sha256", secret).update(payload).digest("hex");
}

/** Verify a webhook signature supplied by the receiver. */
export function verifyWebhookSignature(secret: string, payload: string, sig: string): boolean
{
    const expected = signWebhookPayload(secret, payload);
    // Constant-time comparison to prevent timing attacks.
    return expected.length === sig.length && signWebhookPayload(secret, payload) === sig;
}

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------
type PrismaClient = {
    webhookSubscriber: {
        create: (args: unknown) => Promise<unknown>;
        findMany : (args: unknown) => Promise<unknown[]>;
        findUnique : (args: unknown) => Promise<unknown|null>;
        delete : (args: unknown) => Promise<unknown>;
        update : (args: unknown) => Promise<unknown>;
    };
    webhookDeliveryAttempt : {
        create : (args: unknown) => Promise<unknown>;
        findMany : (args: unknown) => Promise<unknown[]>;
    };
};

// ---------------------------------------------------------------------------
// Routes
// ---------------------------------------------------------------------------

const SubscriberCreateSchema = z.object({
    url : z.string().url(),
    events : z.array(z.string().min(1)).min(1),
});

export async function webhooksRoutes(app: FastifyInstance)
{
    const {prisma} = app as unknown as {prisma : PrismaClient};

    if (!prisma)
    {
        app.log.warn("webhooks routes: prisma not available, skipping registration");
        return;
    }

    /** Register a new webhook subscriber */
    app.post("/webhooks/subscribers", async (req, reply) => {
        const parsed = SubscriberCreateSchema.safeParse(req.body);
        if (!parsed.success)
            return reply.code(400).send({error : parsed.error.flatten()});

        const signingSecret = randomBytes(32).toString("hex");
        const subscriber = await prisma.webhookSubscriber.create({
            data : {
                id : randomUUID(),
                url : parsed.data.url,
                events : parsed.data.events,
                signingSecret,
            },
        });
        // Return the signing secret once — callers must store it.
        return reply.code(201).send({subscriber, signingSecret});
    });

    /** List all subscribers */
    app.get("/webhooks/subscribers", async (_req, _reply) => {
        const subscribers = await prisma.webhookSubscriber.findMany({
            orderBy : {createdAt : "desc"},
            select : {
                id : true,
                url : true,
                events : true,
                active : true,
                createdAt : true,
                // Do NOT return signingSecret
            },
        } as Parameters<PrismaClient["webhookSubscriber"]["findMany"]>[ 0 ]);
        return {subscribers};
    });

    /** Deactivate (soft-delete) a subscriber */
    app.delete("/webhooks/subscribers/:id", async (req, reply) => {
        const {id} = req.params as {id : string};
        const existing = await prisma.webhookSubscriber.findUnique({where : {id}});
        if (!existing)
            return reply.code(404).send({error : "not found"});
        await prisma.webhookSubscriber.update({where : {id}, data : {active : false}});
        return {ok : true};
    });

    /** List recent delivery attempts */
    app.get("/webhooks/deliveries", async (req, _reply) => {
        const {subscriberId, event, limit = "50"} = req.query as Record<string, string>;
        const attempts = await prisma.webhookDeliveryAttempt.findMany({
            where : {
                ...(subscriberId ? {subscriberId} : {}),
                ...(event ? {event} : {}),
            },
            orderBy : {attemptedAt : "desc"},
            take : Math.min(parseInt(limit, 10) || 50, 200),
        });
        return {attempts};
    });
}

// ---------------------------------------------------------------------------
// Event delivery — call from business logic to fan out to subscribers
// ---------------------------------------------------------------------------

/** Deliver an event to all active matching subscribers. Fire-and-forget safe. */
export async function deliverWebhookEvent(
    prisma: PrismaClient,
    event: string,
    payload: Record<string, unknown>,
    ): Promise<void>
{
    const subscribers = await prisma.webhookSubscriber.findMany({
        where : {active : true},
    }) as Array<{id : string; url : string; events : string[]; signingSecret : string}>;

    const matching = subscribers.filter(s => s.events.includes(event) || s.events.includes("*"));

    await Promise.allSettled(
        matching.map(async (sub) => {
            const body = JSON.stringify({event, payload, timestamp : new Date().toISOString()});
            const signature = signWebhookPayload(sub.signingSecret, body);

            let statusCode: number|null = null;
            let responseBody: string|null = null;
            let success = false;

            try
            {
                const res = await fetch(sub.url, {
                    method : "POST",
                    headers : {
                        "Content-Type" : "application/json",
                        "X-Banana-Signature" : signature,
                        "X-Banana-Event" : event,
                    },
                    body,
                    signal : AbortSignal.timeout(10_000),
                });
                statusCode = res.status;
                responseBody = (await res.text()).slice(0, 1000);
                success = res.status >= 200 && res.status < 300;
            }
            catch (err)
            {
                responseBody = err instanceof Error ? err.message : String(err);
            }

            await prisma.webhookDeliveryAttempt.create({
                data : {
                    id : randomUUID(),
                    subscriberId : sub.id,
                    event,
                    payload,
                    statusCode,
                    responseBody,
                    success,
                },
            });
        }),
    );
}
