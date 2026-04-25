import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

import {loadTrainedModel, scoreText} from './model';

const ScoreRequest = z.object({
  text: z.string().min(1),
  threshold: z.number().min(0).max(1).default(0.5),
});

export async function registerNotBananaRoutes(app: FastifyInstance) {
  app.post('/not-banana/score', async (req, reply) => {
    const parsed = ScoreRequest.safeParse(req.body);
    if (!parsed.success) {
      return reply.status(400).send({
        error: 'invalid_argument',
        issues: parsed.error.flatten(),
      });
    }

    const model = loadTrainedModel();
    const scored = scoreText(parsed.data.text, model);

    if (scored.token_count === 0) {
      return reply.status(400).send({
        error: 'invalid_argument',
        message: 'text must contain signal tokens',
      });
    }

    const label =
        scored.banana_score >= parsed.data.threshold ? 'banana' : 'not_banana';

    return reply.status(200).send({
      label,
      banana_score: Number(scored.banana_score.toFixed(4)),
      threshold: parsed.data.threshold,
      matched_tokens: scored.matched_banana_tokens,
      matched_not_banana_tokens: scored.matched_not_banana_tokens,
      token_count: scored.token_count,
      model: {
        source: model.source,
        vocab_size: model.vocab_size,
        artifact_path: model.artifact_path,
        generated_at_utc: model.generated_at_utc,
      },
    });
  });

  app.get('/not-banana/model', async (_req, reply) => {
    const model = loadTrainedModel();
    return reply.status(200).send({
      source: model.source,
      vocab_size: model.vocab_size,
      artifact_path: model.artifact_path,
      generated_at_utc: model.generated_at_utc,
      banana_token_sample: Array.from(model.banana_tokens).slice(0, 20).sort(),
      not_banana_token_sample:
          Array.from(model.not_banana_tokens).slice(0, 20).sort(),
    });
  });
}
