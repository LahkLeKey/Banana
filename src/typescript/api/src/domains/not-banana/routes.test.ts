import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';
import {mkdtempSync, rmSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';

import {__resetTrainedModelCacheForTests} from './model';
import {registerNotBananaRoutes} from './routes';

function createVocabularyArtifact(
    entries: Array<{token: string; weight: number}>):
    {dir: string; filePath: string} {
  const dir = mkdtempSync(join(tmpdir(), 'banana-routes-model-'));
  const filePath = join(dir, 'vocabulary.json');
  writeFileSync(
      filePath, JSON.stringify({
        schema_version: 1,
        generated_at_utc: '2026-04-25T00:00:00Z',
        vocab_size: entries.length,
        vocabulary: entries.map((entry) => ({
                                  token: entry.token,
                                  banana_count: entry.weight > 0 ? 1 : 0,
                                  not_banana_count: entry.weight < 0 ? 1 : 0,
                                  support: 1,
                                  weight: entry.weight,
                                  signal: 1,
                                })),
      }),
      'utf8');
  return {dir, filePath};
}

async function createApp() {
  const app = Fastify({logger: false});
  await registerNotBananaRoutes(app);
  await app.ready();
  return app;
}

describe('not-banana routes', () => {
  test(
      'scores text with trained artifact and exposes model metadata',
      async () => {
        const artifact = createVocabularyArtifact([
          {token: 'banana', weight: 1},
          {token: 'ripe', weight: 1},
          {token: 'plastic', weight: -1},
        ]);

        process.env.BANANA_NOT_BANANA_MODEL_PATH = artifact.filePath;
        __resetTrainedModelCacheForTests();

        const app = await createApp();

        try {
          const score = await app.inject({
            method: 'POST',
            url: '/not-banana/score',
            payload: {
              text: 'ripe banana plastic',
              threshold: 0.5,
            },
          });

          expect(score.statusCode).toBe(200);
          const scorePayload = score.json() as {
            label: string;
            banana_score: number;
            matched_tokens: string[];
            matched_not_banana_tokens: string[];
            matched_token_count: number;
            model: {source: string; vocab_size: number; artifact_path?: string};
          };
          expect(scorePayload.label).toBe('banana');
          expect(scorePayload.banana_score).toBeGreaterThan(0.5);
          expect(scorePayload.matched_tokens).toContain('banana');
          expect(scorePayload.matched_tokens).toContain('ripe');
          expect(scorePayload.matched_not_banana_tokens).toContain('plastic');
          expect(scorePayload.matched_token_count).toBe(3);
          expect(scorePayload.model.source).toBe('trained-artifact');
          expect(scorePayload.model.vocab_size).toBe(3);

          const model = await app.inject({
            method: 'GET',
            url: '/not-banana/model',
          });

          expect(model.statusCode).toBe(200);
          const modelPayload = model.json() as {
            source: string;
            vocab_size: number;
            artifact_path?: string;
            banana_token_sample: string[];
            not_banana_token_sample: string[];
          };
          expect(modelPayload.source).toBe('trained-artifact');
          expect(modelPayload.vocab_size).toBe(3);
          expect(modelPayload.artifact_path?.endsWith('vocabulary.json'))
              .toBe(true);
          expect(modelPayload.banana_token_sample).toContain('banana');
          expect(modelPayload.not_banana_token_sample).toContain('plastic');
        } finally {
          await app.close();
          rmSync(artifact.dir, {recursive: true, force: true});
        }
      });

  test('rejects invalid score payload', async () => {
    __resetTrainedModelCacheForTests();
    process.env.BANANA_NOT_BANANA_MODEL_PATH = '';

    const app = await createApp();

    try {
      const response = await app.inject({
        method: 'POST',
        url: '/not-banana/score',
        payload: {text: ''},
      });

      expect(response.statusCode).toBe(400);
      const payload = response.json() as {error: string};
      expect(payload.error).toBe('invalid_argument');

      const unknown = await app.inject({
        method: 'POST',
        url: '/not-banana/score',
        payload: {text: 'hello world from keyboard only'},
      });
      expect(unknown.statusCode).toBe(400);
    } finally {
      await app.close();
    }
  });
});
