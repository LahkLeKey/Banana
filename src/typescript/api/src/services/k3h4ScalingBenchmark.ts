import fs from 'node:fs';
import path from 'node:path';

/** One measured point from the native k3h4 vs attention benchmark artifact. */
export type K3h4ScalingBenchmarkEntry = {
  readonly n: number; readonly k3h4Ns: number; readonly attentionNs: number;
};

/** Parsed benchmark artifact returned to API consumers. */
export type K3h4ScalingBenchmarkResult = {
  readonly contractVersion: 1; readonly schemaVersion: number; readonly series: readonly K3h4ScalingBenchmarkEntry[]; readonly metadata: {
    readonly calibratedSizes: readonly number[]; readonly extrapolatedAbove:
                                                              number | null;
  };
};

type RawBenchmarkJson = {
  schema_version: number;
  series: Array<{n: number; k3h4_ns: number; attention_ns: number}>;
};

const ARTIFACT_RELATIVE_PATH =
    path.join('artifacts', 'native', 'k3h4-scaling-benchmark.json');

const SEARCH_ROOTS = [
  process.cwd(),
  path.resolve(process.cwd(), '..'),
  path.resolve(process.cwd(), '../..'),
  path.resolve(process.cwd(), '../../..'),
  '/workspace',
];

/* Searches a few common workspace roots because the API can run from nested
 * packages. */
function resolveArtifactPath(): string|null {
  for (const root of SEARCH_ROOTS) {
    const candidate = path.join(root, ARTIFACT_RELATIVE_PATH);
    if (fs.existsSync(candidate)) {
      return candidate;
    }
  }
  return null;
}

/** Loader result for the optional benchmark artifact exposed by the API. */
export type K3h4ScalingBenchmarkStatus =|{
  kind: 'ok';
  result: K3h4ScalingBenchmarkResult
}
|{kind: 'not_found'}|{
  kind: 'unavailable';
  reason: string
};

/**
 * Loads the generated native benchmark artifact, if present, and normalizes
 * missing or unreadable artifact states into explicit API status variants.
 */
export function loadK3h4ScalingBenchmark(): K3h4ScalingBenchmarkStatus {
  const artifactPath = resolveArtifactPath();
  if (!artifactPath) {
    return {kind: 'not_found'};
  }

  let raw: RawBenchmarkJson;
  try {
    const text = fs.readFileSync(artifactPath, 'utf8');
    raw = JSON.parse(text) as RawBenchmarkJson;
  } catch (err) {
    return {
      kind: 'unavailable',
      reason: err instanceof Error ? err.message : String(err),
    };
  }

  if (!Array.isArray(raw.series) || raw.series.length === 0) {
    return {kind: 'unavailable', reason: 'benchmark artifact has empty series'};
  }

  const series: K3h4ScalingBenchmarkEntry[] =
      raw.series.map((entry) => ({
                       n: entry.n,
                       k3h4Ns: entry.k3h4_ns,
                       attentionNs: entry.attention_ns,
                     }));

  return {
    kind: 'ok',
    result: {
      contractVersion: 1,
      schemaVersion: raw.schema_version ?? 1,
      series,
      metadata: {
        calibratedSizes: series.map((e) => e.n),
        extrapolatedAbove: null,
      },
    },
  };
}
