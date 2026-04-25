// Loader for trained banana vs not-banana vocabulary artifacts produced by
// scripts/train-not-banana-model.py. Falls back to a built-in deterministic
// signal-token set when no artifact is available so runtime stays operational
// in dev / test without any training step.
import {existsSync, readFileSync} from 'node:fs';
import {resolve} from 'node:path';

export type TrainedVocabularyEntry = {
  token: string; banana_count: number; not_banana_count: number;
  support: number;
  weight: number;
  signal: number;
};

export type TrainedModel = {
  source: 'trained-artifact'|'builtin-fallback';
  artifact_path?: string;
  generated_at_utc?: string; vocab_size: number; banana_tokens: Set<string>;
  not_banana_tokens: Set<string>;
  weights: Map<string, number>;
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

function buildBuiltinFallback(): TrainedModel {
  const weights = new Map<string, number>();
  for (const token of BUILTIN_BANANA_TOKENS) weights.set(token, 1);
  for (const token of BUILTIN_NOT_BANANA_TOKENS) weights.set(token, -1);
  return {
    source: 'builtin-fallback',
    vocab_size: BUILTIN_BANANA_TOKENS.length + BUILTIN_NOT_BANANA_TOKENS.length,
    banana_tokens: new Set(BUILTIN_BANANA_TOKENS),
    not_banana_tokens: new Set(BUILTIN_NOT_BANANA_TOKENS),
    weights,
  };
}

function resolveArtifactPath(): string|undefined {
  const explicit = process.env.BANANA_NOT_BANANA_MODEL_PATH;
  if (explicit && explicit.length > 0) return resolve(explicit);

  const candidates = [
    'artifacts/not-banana-model-readiness-check/vocabulary.json',
    'artifacts/not-banana-model/vocabulary.json',
  ];
  for (const candidate of candidates) {
    const full = resolve(candidate);
    if (existsSync(full)) return full;
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

  if (!parsed.vocabulary || !Array.isArray(parsed.vocabulary)) {
    throw new Error(`vocabulary.json missing 'vocabulary' array`);
  }

  const banana = new Set<string>();
  const notBanana = new Set<string>();
  const weights = new Map<string, number>();

  for (const entry of parsed.vocabulary) {
    if (!entry || typeof entry.token !== 'string') continue;
    const token = entry.token.toLowerCase();
    const weight = Number(entry.weight ?? 0);
    weights.set(token, weight);
    if (weight > 0)
      banana.add(token);
    else if (weight < 0)
      notBanana.add(token);
  }

  return {
    source: 'trained-artifact',
    artifact_path: artifactPath,
    generated_at_utc: parsed.generated_at_utc,
    vocab_size: parsed.vocab_size ?? parsed.vocabulary.length,
    banana_tokens: banana,
    not_banana_tokens: notBanana,
    weights,
  };
}

let cached: TrainedModel|undefined;

export function loadTrainedModel(force = false): TrainedModel {
  if (cached && !force) return cached;

  const artifactPath = resolveArtifactPath();
  if (!artifactPath) {
    cached = buildBuiltinFallback();
    return cached;
  }

  try {
    const text = readFileSync(artifactPath, 'utf8');
    cached = parseTrainedVocabulary(text, artifactPath);
  } catch {
    cached = buildBuiltinFallback();
  }
  return cached;
}

export function __resetTrainedModelCacheForTests() {
  cached = undefined;
}

export function tokenize(text: string): string[] {
  return text.toLowerCase()
      .split(/[^a-z0-9']+/)
      .filter((token) => token.length > 0);
}

export type ScoredText = {
  banana_score: number; matched_banana_tokens: string[];
  matched_not_banana_tokens: string[];
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
    token_count: tokens.length,
  };
}
