import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

const RipenessRequest = z.object({
  inputJson: z.string().trim().min(1),
});

export const RipenessResponse = z.object({
  label: z.enum(['ripe', 'unripe', 'overripe']),
  confidence: z.number(),
  model: z.string().min(1),
});

function resolveAspNetBaseUrl(): string {
  return (process.env.BANANA_ASPNET_API_BASE_URL ?? 'http://localhost:8080')
      .replace(/\/$/, '');
}

async function readUpstreamPayload(response: Response): Promise<unknown> {
  const contentType = response.headers.get('content-type') ?? '';
  if (contentType.includes('application/json')) {
    return response.json();
  }

  const text = await response.text();
  return text.length > 0 ? {error: text} : null;
}

export async function registerRipenessRoutes(app: FastifyInstance) {
  app.post('/ripeness/predict', async (req, reply) => {
    const parsed = RipenessRequest.safeParse(req.body);
    if (!parsed.success) {
      return reply.status(400).send({
        error: 'invalid_argument',
        issues: parsed.error.flatten(),
      });
    }

    const upstreamUrl = `${resolveAspNetBaseUrl()}/ripeness/predict`;

    let upstreamResponse: Response;
    try {
      upstreamResponse = await fetch(upstreamUrl, {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify(parsed.data),
      });
    } catch {
      return reply.status(502).send({
        error: 'upstream_unreachable',
        upstream: 'aspnet',
      });
    }

    const upstreamPayload = await readUpstreamPayload(upstreamResponse);

    if (!upstreamResponse.ok) {
      return reply.status(upstreamResponse.status).send(upstreamPayload ?? {
        error: 'upstream_error'
      });
    }

    const payloadValidation = RipenessResponse.safeParse(upstreamPayload);
    if (!payloadValidation.success) {
      return reply.status(502).send({
        error: 'invalid_upstream_payload',
        issues: payloadValidation.error.flatten(),
      });
    }

    return reply.status(200).send(payloadValidation.data);
  });
}
