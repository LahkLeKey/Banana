import {describe, expect, test} from 'bun:test';
import Fastify from 'fastify';
import {mkdtempSync, rmSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';

import {__resetTrainedModelCacheForTests} from './model';
import {registerNotBananaRoutes} from './routes';

function createVocabularyArtifact(
    entries: Array<{token: string; weight: number}>, options?: {
      minSignalScore?: number;
      trainingProfile?: string;
      sessionMode?: string;
    }): {dir: string; filePath: string} {
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

  if (options &&
      (options.minSignalScore !== undefined ||
       options.trainingProfile !== undefined ||
       options.sessionMode !== undefined)) {
    writeFileSync(
        join(dir, 'metrics.json'), JSON.stringify({
          schema_version: 1,
          generated_at_utc: '2026-04-25T00:00:00Z',
          metrics: {
            min_signal_score: options.minSignalScore,
            training_profile: options.trainingProfile,
            session_mode: options.sessionMode,
          },
        }),
        'utf8');
  }

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
        const artifact = createVocabularyArtifact(
            [
              {token: 'banana', weight: 1},
              {token: 'ripe', weight: 1},
              {token: 'plastic', weight: -1},
            ],
            {
              minSignalScore: 0.75,
              trainingProfile: 'ci',
              sessionMode: 'single',
            });

        process.env.BANANA_NOT_BANANA_MODEL_PATH = artifact.filePath;
        __resetTrainedModelCacheForTests();

        const app = await createApp();

        try {
          const scoreDefaultThreshold = await app.inject({
            method: 'POST',
            url: '/not-banana/score',
            payload: {
              text: 'ripe banana plastic',
            },
          });

          expect(scoreDefaultThreshold.statusCode).toBe(200);
          const scoreDefaultPayload = scoreDefaultThreshold.json() as {
            label: string;
            banana_score: number;
            threshold: number;
            threshold_source: string;
            matched_tokens: string[];
            matched_not_banana_tokens: string[];
            matched_token_count: number;
            model: {
              source: string; vocab_size: number;
              artifact_path?: string; recommended_threshold: number;
            };
          };
          expect(scoreDefaultPayload.label).toBe('not_banana');
          expect(scoreDefaultPayload.banana_score).toBeGreaterThan(0.5);
          expect(scoreDefaultPayload.threshold).toBe(0.75);
          expect(scoreDefaultPayload.threshold_source).toBe('model');
          expect(scoreDefaultPayload.matched_tokens).toContain('banana');
          expect(scoreDefaultPayload.matched_tokens).toContain('ripe');
          expect(scoreDefaultPayload.matched_not_banana_tokens)
              .toContain('plastic');
          expect(scoreDefaultPayload.matched_token_count).toBe(3);
          expect(scoreDefaultPayload.model.source).toBe('trained-artifact');
          expect(scoreDefaultPayload.model.vocab_size).toBe(3);
          expect(scoreDefaultPayload.model.recommended_threshold).toBe(0.75);

          const scoreOverrideThreshold = await app.inject({
            method: 'POST',
            url: '/not-banana/score',
            payload: {
              text: 'ripe banana plastic',
              threshold: 0.5,
            },
          });

          expect(scoreOverrideThreshold.statusCode).toBe(200);
          const scoreOverridePayload = scoreOverrideThreshold.json() as {
            label: string;
            threshold: number;
            threshold_source: string;
          };
          expect(scoreOverridePayload.label).toBe('banana');
          expect(scoreOverridePayload.threshold).toBe(0.5);
          expect(scoreOverridePayload.threshold_source).toBe('request');

          const model = await app.inject({
            method: 'GET',
            url: '/not-banana/model',
          });

          expect(model.statusCode).toBe(200);
          const modelPayload = model.json() as {
            source: string;
            vocab_size: number;
            artifact_path?: string;
            recommended_threshold: number;
            metrics?: {training_profile?: string; session_mode?: string};
            banana_token_sample: string[];
            not_banana_token_sample: string[];
          };
          expect(modelPayload.source).toBe('trained-artifact');
          expect(modelPayload.vocab_size).toBe(3);
          expect(modelPayload.artifact_path?.endsWith('vocabulary.json'))
              .toBe(true);
          expect(modelPayload.recommended_threshold).toBe(0.75);
          expect(modelPayload.metrics?.training_profile).toBe('ci');
          expect(modelPayload.metrics?.session_mode).toBe('single');
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
