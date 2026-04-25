import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

const BananaSignalTokens = new Set([
  'banana',
  'ripe',
  'peel',
  'smoothie',
  'plantation',
  'harvest',
  'bunch',
  'cavendish',
  'fruit',
  'crate',
  'bread',
  'cartons',
]);

const ScoreRequest = z.object({
  text: z.string().min(1),
  threshold: z.number().min(0).max(1).default(0.5),
});

function tokenize(text: string): string[] {
  return text.toLowerCase()
      .split(/[^a-z0-9']+/)
      .filter((token) => token.length > 0);
}

export async function registerNotBananaRoutes(app: FastifyInstance) {
  app.post('/not-banana/score', async (req, reply) => {
    const parsed = ScoreRequest.safeParse(req.body);
    if (!parsed.success) {
      return reply.status(400).send({
        error: 'invalid_argument',
        issues: parsed.error.flatten(),
      });
    }

    const tokens = tokenize(parsed.data.text);
    if (tokens.length === 0) {
      return reply.status(400).send({
        error: 'invalid_argument',
        message: 'text must contain signal tokens'
      });
    }

    const matched = tokens.filter((token) => BananaSignalTokens.has(token));
    const bananaScore = matched.length / tokens.length;
    const label =
        bananaScore >= parsed.data.threshold ? 'banana' : 'not_banana';

    return reply.status(200).send({
      label,
      banana_score: Number(bananaScore.toFixed(4)),
      threshold: parsed.data.threshold,
      matched_tokens: Array.from(new Set(matched)).sort(),
      token_count: tokens.length,
    });
  });
}
