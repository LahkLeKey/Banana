// Loader for trained banana vs not-banana vocabulary artifacts produced by
// scripts/train-not-banana-model.py. Falls back to a built-in deterministic
// signal-token set when no artifact is available so runtime stays operational
// in dev / test without any training step.
import {existsSync, readFileSync} from 'node:fs';
import {dirname, resolve} from 'node:path';

export const TRAINING_ARTIFACT_SCHEMA_VERSION = 1;

export type TrainedVocabularyEntry = {
  token: string; banana_count: number; not_banana_count: number;
  support: number;
  weight: number;
  signal: number;
};

export type TrainedModel = {
  source: 'trained-artifact'|'builtin-fallback';
  artifact_path?: string;
  fallback_reason?: string;
  generated_at_utc?: string; vocab_size: number; banana_tokens: Set<string>;
  not_banana_tokens: Set<string>;
  recommended_threshold: number;
  metrics?: ModelMetrics; weights: Map<string, number>;
};

export type ModelMetrics = {
  min_signal_score?: number;
  min_f1?: number;
  holdout_f1?: number;
  holdout_accuracy?: number;
  training_profile?: string;
  session_mode?: string;
  selected_session?: number;
};

const BUILTIN_BANANA_TOKENS = [
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
];

const BUILTIN_NOT_BANANA_TOKENS = [
  'plastic',
  'battery',
  'engine',
  'asphalt',
  'steel',
  'concrete',
  'garbage',
  'waste',
  'junk',
  'noise',
];

const STOPWORDS = new Set([
  'a',  'an', 'and', 'are', 'as', 'at',   'be',  'by',   'for', 'from', 'in',
  'is', 'it', 'of',  'on',  'or', 'that', 'the', 'this', 'to',  'with',
]);

function clamp01(value: number): number {
  if (value < 0) return 0;
  if (value > 1) return 1;
  return value;
}

function buildBuiltinFallback(reason?: string): TrainedModel {
  const weights = new Map<string, number>();
  for (const token of BUILTIN_BANANA_TOKENS) weights.set(token, 1);
  for (const token of BUILTIN_NOT_BANANA_TOKENS) weights.set(token, -1);
  return {
    source: 'builtin-fallback',
    fallback_reason: reason,
    vocab_size: BUILTIN_BANANA_TOKENS.length + BUILTIN_NOT_BANANA_TOKENS.length,
    banana_tokens: new Set(BUILTIN_BANANA_TOKENS),
    not_banana_tokens: new Set(BUILTIN_NOT_BANANA_TOKENS),
    recommended_threshold: 0.5,
    weights,
  };
}

function readModelMetrics(metricsPath: string): ModelMetrics {
  if (!existsSync(metricsPath)) {
    throw new Error('missing_metrics_artifact');
  }

  try {
    const parsed = JSON.parse(readFileSync(metricsPath, 'utf8')) as {
      schema_version?: number;
      metrics?: {
        min_signal_score?: number;
        min_f1?: number;
        holdout_f1?: number;
        holdout_accuracy?: number;
        training_profile?: string;
        session_mode?: string;
        selected_session?: number;
      };
    };

    if (parsed.schema_version !== TRAINING_ARTIFACT_SCHEMA_VERSION) {
      throw new Error(`unsupported_metrics_schema_version:${
          String(parsed.schema_version)}`);
    }

    const metrics = parsed.metrics;
    if (!metrics || typeof metrics !== 'object') {
      throw new Error('metrics_payload_missing');
    }

    return {
      min_signal_score: typeof metrics.min_signal_score === 'number' ?
          clamp01(metrics.min_signal_score) :
          undefined,
      min_f1: typeof metrics.min_f1 === 'number' ? clamp01(metrics.min_f1) :
                                                   undefined,
      holdout_f1: typeof metrics.holdout_f1 === 'number' ?
          clamp01(metrics.holdout_f1) :
          undefined,
      holdout_accuracy: typeof metrics.holdout_accuracy === 'number' ?
          clamp01(metrics.holdout_accuracy) :
          undefined,
      training_profile: metrics.training_profile,
      session_mode: metrics.session_mode,
      selected_session: metrics.selected_session,
    };
  } catch (error) {
    if (error instanceof Error) throw error;
    throw new Error('invalid_metrics_artifact');
  }
}

function resolveArtifactPath(): string|undefined {
  const explicit = process.env.BANANA_NOT_BANANA_MODEL_PATH;
  if (explicit && explicit.length > 0) return resolve(explicit);

  const candidates = [
    'artifacts/not-banana-model-readiness-check/vocabulary.json',
    'artifacts/not-banana-model/vocabulary.json',
  ];

  // Search from both current working directory and this module's directory
  // so API execution from nested paths can still discover repo-level artifacts.
  const roots = new Set<string>();

  function addAncestors(start: string, maxDepth = 12) {
    let current = resolve(start);
    for (let i = 0; i < maxDepth; i += 1) {
      roots.add(current);
      const parent = resolve(current, '..');
      if (parent === current) break;
      current = parent;
    }
  }

  addAncestors(process.cwd(), 10);
  addAncestors(import.meta.dir, 14);

  for (const root of roots) {
    for (const candidate of candidates) {
      const full = resolve(root, candidate);
      if (existsSync(full)) return full;
    }
  }

  return undefined;
}

function parseTrainedVocabulary(
    jsonText: string, artifactPath: string): TrainedModel {
  const parsed = JSON.parse(jsonText) as {
    schema_version?: number;
    generated_at_utc?: string;
    vocab_size?: number;
    vocabulary?: TrainedVocabularyEntry[];
  };

  if (parsed.schema_version !== TRAINING_ARTIFACT_SCHEMA_VERSION) {
    throw new Error(`unsupported_vocabulary_schema_version:${
        String(parsed.schema_version)}`);
  }

  if (typeof parsed.generated_at_utc !== 'string' ||
      parsed.generated_at_utc.length === 0) {
    throw new Error('missing_generated_at_utc');
  }

  if (!parsed.vocabulary || !Array.isArray(parsed.vocabulary)) {
    throw new Error(`vocabulary.json missing 'vocabulary' array`);
  }

  if (typeof parsed.vocab_size === 'number' &&
      parsed.vocab_size !== parsed.vocabulary.length) {
    throw new Error(`vocab_size_mismatch:declared=${parsed.vocab_size},actual=${
        parsed.vocabulary.length}`);
  }

  const banana = new Set<string>();
  const notBanana = new Set<string>();
  const weights = new Map<string, number>();

  for (let index = 0; index < parsed.vocabulary.length; index += 1) {
    const entry = parsed.vocabulary[index];
    if (!entry || typeof entry.token !== 'string' ||
        entry.token.trim().length === 0) {
      throw new Error(`invalid_vocabulary_entry_token:index=${index}`);
    }

    if (typeof entry.weight !== 'number' || !Number.isFinite(entry.weight)) {
      throw new Error(`invalid_vocabulary_entry_weight:index=${index}`);
    }

    const token = entry.token.toLowerCase();
    const weight = Number(entry.weight ?? 0);
    weights.set(token, weight);
    if (weight > 0)
      banana.add(token);
    else if (weight < 0)
      notBanana.add(token);
  }

  if (weights.size === 0) {
    throw new Error('empty_vocabulary_weights');
  }

  const metrics =
      readModelMetrics(resolve(dirname(artifactPath), 'metrics.json'));
  const recommendedThreshold = metrics?.min_signal_score ?? 0.5;

  return {
    source: 'trained-artifact',
    artifact_path: artifactPath,
    generated_at_utc: parsed.generated_at_utc,
    vocab_size: parsed.vocab_size ?? parsed.vocabulary.length,
    banana_tokens: banana,
    not_banana_tokens: notBanana,
    recommended_threshold: recommendedThreshold,
    metrics,
    weights,
  };
}

let cached: TrainedModel|undefined;

export function loadTrainedModel(force = false): TrainedModel {
  if (cached && !force) return cached;

  const artifactPath = resolveArtifactPath();
  if (!artifactPath) {
    cached = buildBuiltinFallback('artifact_not_found');
    return cached;
  }

  try {
    const text = readFileSync(artifactPath, 'utf8');
    cached = parseTrainedVocabulary(text, artifactPath);
  } catch (error) {
    cached = buildBuiltinFallback(`artifact_parse_error:${
        error instanceof Error ? error.message : 'unknown'}`);
  }
  return cached;
}

export function __resetTrainedModelCacheForTests() {
  cached = undefined;
}

export function tokenize(text: string): string[] {
  return text.toLowerCase()
      .split(/[^a-z0-9']+/)
      .filter((token) => token.length > 0 && !STOPWORDS.has(token));
}

export type ScoredText = {
  banana_score: number; matched_banana_tokens: string[];
  matched_not_banana_tokens: string[];
  matched_token_count: number;
  token_count: number;
};

export function scoreText(
    text: string, model = loadTrainedModel()): ScoredText {
  const tokens = tokenize(text);
  if (tokens.length === 0) {
    return {
      banana_score: 0.5,
      matched_banana_tokens: [],
      matched_not_banana_tokens: [],
      matched_token_count: 0,
      token_count: 0,
    };
  }

  const matchedBanana: string[] = [];
  const matchedNotBanana: string[] = [];
  let weightSum = 0;
  let weightedHits = 0;

  for (const token of tokens) {
    const weight = model.weights.get(token);
    if (weight === undefined) continue;
    weightSum += Math.abs(weight);
    weightedHits += weight;
    if (weight > 0)
      matchedBanana.push(token);
    else if (weight < 0)
      matchedNotBanana.push(token);
  }

  let bananaScore: number;
  if (weightSum === 0) {
    bananaScore = 0.5;
  } else {
    // Map weighted ratio in [-1, 1] to [0, 1].
    const ratio = weightedHits / weightSum;
    bananaScore = (ratio + 1) / 2;
  }

  return {
    banana_score: bananaScore,
    matched_banana_tokens: Array.from(new Set(matchedBanana)).sort(),
    matched_not_banana_tokens: Array.from(new Set(matchedNotBanana)).sort(),
    matched_token_count: matchedBanana.length + matchedNotBanana.length,
    token_count: tokens.length,
  };
}
