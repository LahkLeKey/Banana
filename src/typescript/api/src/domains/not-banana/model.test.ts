import {describe, expect, test} from 'bun:test';
import {resolve} from 'node:path';

import {__resetTrainedModelCacheForTests, loadTrainedModel, scoreText,} from './model';

const REPO_ROOT = resolve(import.meta.dir, '..', '..', '..', '..', '..', '..');
const TRAINED_ARTIFACT = resolve(
    REPO_ROOT, 'artifacts', 'not-banana-model-readiness-check',
    'vocabulary.json');

describe('not-banana trained model loader', () => {
  test('loads trained vocabulary artifact when present', () => {
    __resetTrainedModelCacheForTests();
    process.env.BANANA_NOT_BANANA_MODEL_PATH = TRAINED_ARTIFACT;

    const model = loadTrainedModel(true);

    expect(model.source).toBe('trained-artifact');
    expect(model.vocab_size).toBeGreaterThan(0);
    expect(model.banana_tokens.has('banana')).toBe(true);

    const positive = scoreText('ripe banana bunch from harvest', model);
    expect(positive.banana_score).toBeGreaterThanOrEqual(0.5);
    expect(positive.matched_banana_tokens).toContain('banana');

    const negative = scoreText('plastic battery cable adapter', model);
    expect(negative.banana_score).toBeLessThan(0.5);
    expect(negative.matched_not_banana_tokens.length).toBeGreaterThan(0);
  });

  test('falls back to builtin tokens when artifact is missing', () => {
    __resetTrainedModelCacheForTests();
    process.env.BANANA_NOT_BANANA_MODEL_PATH =
        'artifacts/does-not-exist/vocabulary.json';

    const model = loadTrainedModel(true);

    expect(model.source).toBe('builtin-fallback');
    expect(model.banana_tokens.has('banana')).toBe(true);
    expect(model.not_banana_tokens.has('plastic')).toBe(true);
  });

  test('returns neutral score for empty token input', () => {
    __resetTrainedModelCacheForTests();
    process.env.BANANA_NOT_BANANA_MODEL_PATH = '';
    const model = loadTrainedModel(true);
    const scored = scoreText('', model);
    expect(scored.token_count).toBe(0);
    expect(scored.banana_score).toBeCloseTo(0.5, 3);
  });
});
