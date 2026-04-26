import {describe, expect, test} from 'bun:test';
import {mkdtempSync, rmSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {dirname, join} from 'node:path';

import {__resetTrainedModelCacheForTests, loadTrainedModel, scoreText,} from './model';

function createVocabularyArtifact(
    entries: Array<{token: string; weight: number}>, options?: {
      minSignalScore?: number;
      trainingProfile?: string;
      sessionMode?: string;
    }): string {
  const dir = mkdtempSync(join(tmpdir(), 'banana-model-test-'));
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
    const metricsPath = join(dir, 'metrics.json');
    writeFileSync(
        metricsPath, JSON.stringify({
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

  return filePath;
}

describe('not-banana trained model loader', () => {
  test('loads trained vocabulary artifact when present', () => {
    __resetTrainedModelCacheForTests();
    const artifact = createVocabularyArtifact(
        [
          {token: 'banana', weight: 1},
          {token: 'ripe', weight: 1},
          {token: 'plastic', weight: -1},
          {token: 'battery', weight: -1},
        ],
        {
          minSignalScore: 0.8,
          trainingProfile: 'ci',
          sessionMode: 'single',
        });
    try {
      process.env.BANANA_NOT_BANANA_MODEL_PATH = artifact;

      const model = loadTrainedModel(true);

      expect(model.source).toBe('trained-artifact');
      expect(model.vocab_size).toBeGreaterThan(0);
      expect(model.banana_tokens.has('banana')).toBe(true);
      expect(model.recommended_threshold).toBeCloseTo(0.8, 4);
      expect(model.metrics?.training_profile).toBe('ci');
      expect(model.metrics?.session_mode).toBe('single');

      const positive = scoreText('ripe banana bunch from harvest', model);
      expect(positive.banana_score).toBeGreaterThanOrEqual(0.5);
      expect(positive.matched_banana_tokens).toContain('banana');

      const negative = scoreText('plastic battery cable adapter', model);
      expect(negative.banana_score).toBeLessThan(0.5);
      expect(negative.matched_not_banana_tokens.length).toBeGreaterThan(0);
    } finally {
      rmSync(dirname(artifact), {recursive: true, force: true});
    }
  });

  test('falls back to builtin tokens when artifact is missing', () => {
    __resetTrainedModelCacheForTests();
    process.env.BANANA_NOT_BANANA_MODEL_PATH =
        'artifacts/does-not-exist/vocabulary.json';

    const model = loadTrainedModel(true);

    expect(model.source).toBe('builtin-fallback');
    expect(model.banana_tokens.has('banana')).toBe(true);
    expect(model.not_banana_tokens.has('plastic')).toBe(true);
    expect(model.recommended_threshold).toBe(0.5);
    expect(model.metrics).toBeUndefined();
  });

  test('returns neutral score for empty token input', () => {
    __resetTrainedModelCacheForTests();
    process.env.BANANA_NOT_BANANA_MODEL_PATH = '';
    const model = loadTrainedModel(true);
    const scored = scoreText('', model);
    expect(scored.token_count).toBe(0);
    expect(scored.matched_token_count).toBe(0);
    expect(scored.banana_score).toBeCloseTo(0.5, 3);
  });
});
