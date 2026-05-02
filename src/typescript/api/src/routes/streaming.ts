/**
 * streaming.ts — SSE verdict streaming endpoint (spec #054).
 *
 * GET /api/v1/streaming/verdict?text=<text>
 *
 * Emits:
 *   data: {"type":"progress","step":"input_validation","pct":25}
 *   data: {"type":"progress","step":"ensemble_scoring","pct":75}
 *   data: {"type":"complete","verdict":{...}}
 */
import type {FastifyInstance} from 'fastify';

function resolveAspNetBaseUrl(): string {
  return (process.env.BANANA_ASPNET_API_BASE_URL ?? 'http://localhost:8080').replace(/\/$/, '');
}

function sseEvent(data: unknown): string {
  return `data: ${JSON.stringify(data)}\n\n`;
}

export async function registerStreamingRoutes(app: FastifyInstance): Promise<void> {
  app.get('/api/v1/streaming/verdict', async (request, reply) => {
    const text = ((request.query as {text?: string}).text ?? '').trim();
    if (!text) {
      return reply.status(400).send({error: 'text query parameter is required'});
    }

    reply.raw.setHeader('Content-Type', 'text/event-stream');
    reply.raw.setHeader('Cache-Control', 'no-cache');
    reply.raw.setHeader('Connection', 'keep-alive');
    reply.raw.setHeader('X-Accel-Buffering', 'no');
    reply.raw.flushHeaders();

    reply.raw.write(sseEvent({type: 'progress', step: 'input_validation', pct: 25}));

    // Proxy to ASP.NET ensemble for actual ML scoring
    let verdict: unknown = {label: 'unknown', score: 0, status: 'error'};
    try {
      const upstreamUrl = `${resolveAspNetBaseUrl()}/ml/ensemble`;
      const upstreamRes = await fetch(upstreamUrl, {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({inputJson: JSON.stringify({text})}),
      });
      if (upstreamRes.ok) {
        verdict = await upstreamRes.json();
      }
    } catch {
      // emit degraded verdict rather than closing the stream with an error
    }

    reply.raw.write(sseEvent({type: 'progress', step: 'ensemble_scoring', pct: 75}));
    reply.raw.write(sseEvent({type: 'complete', verdict}));
    reply.raw.end();
    return reply;
  });
}
