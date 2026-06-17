import nodeFs from 'node:fs';
import fs from 'node:fs/promises';
import path from 'node:path';

export type TrainingSessionMode = 'multiplicative'|'power';

export type SessionStatus = 'pending'|'active'|'completed';

export type TrainingSession = {
  readonly sessionId: string; readonly mode: TrainingSessionMode; readonly createdAtUtc:
                                                                               string;
};

export type EpochConfidence = {
  readonly epochIndex: number; readonly confidence: number; readonly mode:
                                                                         TrainingSessionMode;
};

export type ConfidenceTimeSeries = {
  readonly contractVersion: 1; readonly sessionId: string; readonly status: SessionStatus; readonly mode: TrainingSessionMode; readonly epochs: readonly EpochConfidence[]; readonly metadata: {
    readonly peakEpoch: number | null; readonly rollingAverage3: number | null; readonly defaultMode:
                                                                                             'power';
  };
};

export class K3h4VizServiceError extends Error {
  public readonly code: string;
  public readonly statusCode: number;

  constructor(code: string, message: string, statusCode: number) {
    super(message);
    this.name = 'K3h4VizServiceError';
    this.code = code;
    this.statusCode = statusCode;
  }
}

export type Point2d = {
  readonly x: number; readonly y: number
};

export type ClusterGeometry2d = {
  readonly clusterId: number; readonly centroid: Point2d; readonly inscribedRadius: number; readonly weightedScore: number; readonly spectralProxy: number; readonly polygon: ReadonlyArray<
                                                                                                                                                                         Point2d>;
};

export type TokenGeometry2d = {
  readonly tokenId: string; readonly x: number; readonly y: number; readonly clusterId: number; readonly weightedScore:
                                                                                                             number;
};

export type ProjectionMetadata = {
  readonly method: 'pca'; readonly components: 2; readonly explainedVariance: number; readonly dimensionality:
                                                                                                   number;
};

export type EpochGeometry = {
  readonly contractVersion: 1; readonly sessionId: string; readonly epoch: number; readonly mode: TrainingSessionMode; readonly spectralActive: boolean; readonly clusters: ReadonlyArray<ClusterGeometry2d>; readonly tokens: ReadonlyArray<TokenGeometry2d>; readonly projectionMetadata:
                                                                                                                                                                                                                                                                            ProjectionMetadata;
};

export type EpochPersistenceAnalyticsSnapshot = {
  readonly k3h4Projection: {
    readonly alignment: number;
    readonly radialStability: number;
    readonly nodes: ReadonlyArray<{readonly x: number; readonly y: number;}>;
  };
  readonly k3h4: {
    readonly centers: ReadonlyArray<{readonly clusterId: number}>;
    readonly radii: ReadonlyArray<{
      readonly clusterId: number;
      readonly inscribedRadiusQ16: number;
    }>;
    readonly weightedVoronoiScores: ReadonlyArray<{
      readonly clusterId: number;
      readonly weightedScoreQ16: number;
    }>;
    readonly spectralProxy: ReadonlyArray<{
      readonly clusterId: number;
      readonly amplitudeProxyQ16: number;
    }>;
  };
  readonly k3h4Runtime: {
    readonly spectralActivation: 'disabled'|'affinity-graph';
  };
};

export type RecordedEpochResult = {
  readonly epochIndex: number; readonly persistedAtUtc: string;
};

export interface K3h4TrainingService {
  createTrainingSession(mode: TrainingSessionMode): Promise<TrainingSession>;
  recordEpochFromAnalytics?(
      sessionId: string,
      mode: TrainingSessionMode,
      confidence: number,
      analytics: EpochPersistenceAnalyticsSnapshot,
      ): Promise<RecordedEpochResult>;
  readConfidenceTimeSeries(sessionId: string, mode: TrainingSessionMode):
      Promise<ConfidenceTimeSeries>;
  readEpochGeometry(
      sessionId: string, epochIndex: number,
      mode: TrainingSessionMode): Promise<EpochGeometry>;
}

const ARTIFACT_RELATIVE_PATH =
    path.join('artifacts', 'native', 'k3h4-training');
const TRAINING_ARTIFACT_MAGIC = 0x01;
const TRAINING_ARTIFACT_VERSION = 1;
const INTEGRITY_MARKER = 0xDEADBEEF;

const SEARCH_ROOTS = [
  process.cwd(),
  path.resolve(process.cwd(), '..'),
  path.resolve(process.cwd(), '../..'),
  path.resolve(process.cwd(), '../../..'),
  '/workspace',
];

function resolveTrainingArtifactRoot(): string {
  for (const root of SEARCH_ROOTS) {
    const candidate = path.join(root, ARTIFACT_RELATIVE_PATH);
    if (nodeFs.existsSync(candidate)) {
      return candidate;
    }
  }

  return path.join(process.cwd(), ARTIFACT_RELATIVE_PATH);
}

function validateSessionId(sessionId: string): void {
  if (!/^[A-Za-z0-9_-]{1,15}$/.test(sessionId)) {
    throw new K3h4VizServiceError(
        'invalid_session_id',
        'Session ID must be 1-15 characters using letters, digits, hyphen, or underscore.',
        400,
    );
  }
}

function createSessionId(): string {
  const suffix = Math.floor(Math.random() * 100000).toString().padStart(5, '0');
  return `s${Date.now().toString(36).slice(-6)}${suffix.slice(0, 2)}`;
}

function readInt32LE(buffer: Buffer, offset: number): number {
  return buffer.readInt32LE(offset);
}

function readUInt32LE(buffer: Buffer, offset: number): number {
  return buffer.readUInt32LE(offset);
}

function decodeArtifactConfidence(
    buffer: Buffer, mode: TrainingSessionMode): number {
  if (buffer.length < 51) {
    throw new K3h4VizServiceError(
        'artifact_incomplete', 'Training artifact is truncated.', 422);
  }

  const magic = buffer.readUInt8(0);
  const version = buffer.readUInt16LE(1);
  const clusterCount = readInt32LE(buffer, 35);
  const dimension = readInt32LE(buffer, 39);

  if (magic !== TRAINING_ARTIFACT_MAGIC ||
      version !== TRAINING_ARTIFACT_VERSION) {
    throw new K3h4VizServiceError(
        'version_unsupported', 'Unsupported training artifact version.', 422);
  }

  if (clusterCount < 1 || dimension < 1) {
    throw new K3h4VizServiceError(
        'artifact_incomplete', 'Training artifact has invalid dimensions.',
        422);
  }

  const clusterRecordBytes = dimension * 4 + 16;
  const trailerOffset = 43 + clusterRecordBytes * clusterCount;
  const crcOffset = trailerOffset + 4;

  if (buffer.length < crcOffset + 4) {
    throw new K3h4VizServiceError(
        'artifact_incomplete', 'Training artifact trailer is missing.', 422);
  }

  const marker = readUInt32LE(buffer, trailerOffset);
  if (marker !== INTEGRITY_MARKER) {
    throw new K3h4VizServiceError(
        'artifact_incomplete', 'Training artifact integrity marker missing.',
        422);
  }

  const scoreOffsetWithinCluster =
      dimension * 4 + 4 + (mode === 'power' ? 4 : 0);

  let sum = 0;
  for (let cluster = 0; cluster < clusterCount; cluster += 1) {
    const clusterBase = 43 + cluster * clusterRecordBytes;
    const scoreQ16 =
        readInt32LE(buffer, clusterBase + scoreOffsetWithinCluster);
    sum += scoreQ16 / 65536;
  }

  return sum / clusterCount;
}

function computeRollingAverage3(values: readonly number[]): number|null {
  if (values.length < 3) return null;
  const slice = values.slice(values.length - 3);
  return Number((slice[0] + slice[1] + slice[2]) / 3);
}

function clampUnit(value: number): number {
  if (!Number.isFinite(value)) {
    return 0;
  }
  return Math.min(1, Math.max(0, value));
}

function q16ToFloat(value: number): number {
  return Number.isFinite(value) ? value / 65536 : 0;
}

/** Build a regular octagon centred at (cx, cy) with the given radius. */
function octagonPolygon(cx: number, cy: number, r: number): Point2d[] {
  return Array.from({length: 8}, (_, i) => {
    const angle = (i / 8) * Math.PI * 2;
    return {x: cx + r * Math.cos(angle), y: cy + r * Math.sin(angle)};
  });
}

/**
 * Minimal 2-component PCA via power iteration.
 *
 * Works for small d (≤ 16) and small n (≤ 4 clusters). Returns the
 * 2D projections of each point plus the explained-variance ratio of the
 * first two principal components.
 */
function pca2d(points: readonly(readonly number[])[]): {
  projected: Array<Point2d>; explainedVariance: number; dimensionality: number;
} {
  const n = points.length;
  const d = n > 0 ? points[0].length : 0;
  const dimensionality = d;

  if (n === 0 || d === 0) {
    return {projected: [], explainedVariance: 1, dimensionality};
  }
  if (d === 1) {
    return {
      projected: points.map(p => ({x: p[0] ?? 0, y: 0})),
      explainedVariance: 1,
      dimensionality,
    };
  }

  // Compute mean then centre.
  const mean = Array.from(
      {length: d}, (_, j) => points.reduce((s, p) => s + (p[j] ?? 0), 0) / n);
  const centred = points.map(
      p => Array.from({length: d}, (_, j) => (p[j] ?? 0) - (mean[j] ?? 0)));

  // d×d covariance matrix.
  const cov: number[][] = Array.from(
      {length: d},
      (_, i) => Array.from(
          {length: d},
          (_, j) => centred.reduce((s, p) => s + (p[i] ?? 0) * (p[j] ?? 0), 0) /
              Math.max(n - 1, 1),
          ),
  );

  // Power iteration to find the top eigenvector, then deflate for the second.
  function powerIter(
      matrix: number[][], dim: number, deflate?: readonly number[]): number[] {
    let v: number[] = Array.from({length: dim}, (_, i) => i === 0 ? 1 : 0);
    for (let iter = 0; iter < 120; iter++) {
      let w = Array.from(
          {length: dim},
          (_, i) =>
              (matrix[i] ?? []).reduce((s, mij, j) => s + mij * (v[j] ?? 0), 0),
      );
      if (deflate) {
        const proj = w.reduce((s, wi, i) => s + wi * (deflate[i] ?? 0), 0);
        w = w.map((wi, i) => wi - proj * (deflate[i] ?? 0));
      }
      const norm = Math.sqrt(w.reduce((s, wi) => s + wi * wi, 0));
      if (norm < 1e-12) break;
      v = w.map(wi => wi / norm);
    }
    return v;
  }

  const pc1 = powerIter(cov, d);
  const pc2 = powerIter(cov, d, pc1);

  // Variance captured by each PC.
  const varOf = (pc: readonly number[]) => centred.reduce((s, p) => {
    const proj = p.reduce((sum, pi, i) => sum + pi * (pc[i] ?? 0), 0);
    return s + proj * proj;
  }, 0) / Math.max(n - 1, 1);

  const totalVar = cov.reduce((s, row, i) => s + (row[i] ?? 0), 0);
  const capturedVar = varOf(pc1) + varOf(pc2);
  const explainedVariance = totalVar > 1e-12 ? capturedVar / totalVar : 1;

  const projected =
      centred.map(p => ({
                    x: p.reduce((s, pi, i) => s + pi * (pc1[i] ?? 0), 0),
                    y: p.reduce((s, pi, i) => s + pi * (pc2[i] ?? 0), 0),
                  }));

  return {
    projected,
    explainedVariance: Math.min(1, Math.max(0, explainedVariance)),
    dimensionality
  };
}

/**
 * Decode a training artifact binary into an EpochGeometry response.
 *
 * Binary layout (from native-k3h4-export-abi.md):
 *   Offset  0: artifact_type (uint8)
 *   Offset  1: contract_version (uint16 LE)
 *   Offset  3: byte_order_tag (uint32 LE)
 *   Offset  7: session_id (char[16])
 *   Offset 23: epoch_index (int32 LE)
 *   Offset 27: mode_flag (int32 LE)
 *   Offset 31: spectral_active (int32 LE)
 *   Offset 35: cluster_count (int32 LE)
 *   Offset 39: dimension (int32 LE)
 *   Offset 43: clusters[] — cluster_count × (d*4 + 16) bytes
 *     Per cluster: center[d] float32, inscribed_radius float32,
 *                  multiplicative_score int32 Q16, power_score int32 Q16,
 *                  spectral_proxy int32 Q16
 *   Trailer: integrity_marker (uint32), crc32 (uint32)
 */
function decodeArtifactGeometry(
    buffer: Buffer,
    sessionId: string,
    epochIndex: number,
    mode: TrainingSessionMode,
    ): EpochGeometry {
  if (buffer.length < 51) {
    throw new K3h4VizServiceError(
        'artifact_incomplete', 'Training artifact is truncated.', 422);
  }

  const magic = buffer.readUInt8(0);
  const version = buffer.readUInt16LE(1);

  if (magic !== TRAINING_ARTIFACT_MAGIC ||
      version !== TRAINING_ARTIFACT_VERSION) {
    throw new K3h4VizServiceError(
        'version_unsupported', 'Unsupported training artifact version.', 422);
  }

  const clusterCount = readInt32LE(buffer, 35);
  const dimension = readInt32LE(buffer, 39);
  const modeFlagRaw = readInt32LE(buffer, 27);
  const spectralActiveRaw = readInt32LE(buffer, 31);

  if (clusterCount < 1 || clusterCount > 16 || dimension < 1 ||
      dimension > 16) {
    throw new K3h4VizServiceError(
        'artifact_incomplete',
        'Training artifact has invalid cluster/dimension counts.', 422);
  }

  const clusterRecordBytes = dimension * 4 + 16;
  const trailerOffset = 43 + clusterRecordBytes * clusterCount;

  if (buffer.length < trailerOffset + 8) {
    throw new K3h4VizServiceError(
        'artifact_incomplete', 'Training artifact trailer is missing.', 422);
  }

  const marker = readUInt32LE(buffer, trailerOffset);
  if (marker !== INTEGRITY_MARKER) {
    throw new K3h4VizServiceError(
        'artifact_incomplete',
        'Training artifact integrity marker is absent or corrupt.', 422);
  }

  // Decode cluster centers (float32) and scores.
  const centersRaw: number[][] = [];
  const inscribedRadii: number[] = [];
  const weightedScores: number[] = [];
  const spectralProxies: number[] = [];

  for (let c = 0; c < clusterCount; c++) {
    const base = 43 + c * clusterRecordBytes;
    const center: number[] = [];
    for (let dim = 0; dim < dimension; dim++) {
      center.push(buffer.readFloatLE(base + dim * 4));
    }
    centersRaw.push(center);
    inscribedRadii.push(buffer.readFloatLE(base + dimension * 4));

    const multScore = readInt32LE(buffer, base + dimension * 4 + 4);
    const powScore = readInt32LE(buffer, base + dimension * 4 + 8);
    const specProxy = readInt32LE(buffer, base + dimension * 4 + 12);

    weightedScores.push((mode === 'power' ? powScore : multScore) / 65536);
    spectralProxies.push(specProxy / 65536);
  }

  // Project to 2D via PCA.
  const {projected, explainedVariance, dimensionality} = pca2d(centersRaw);

  // Build cluster geometry (polygon = octagon at inscribed radius).
  const clusters: ClusterGeometry2d[] = centersRaw.map((_, i) => {
    const pt = projected[i] ?? {x: 0, y: 0};
    const r = inscribedRadii[i] ?? 0;
    return {
      clusterId: i,
      centroid: pt,
      inscribedRadius: r,
      weightedScore: weightedScores[i] ?? 0,
      spectralProxy: spectralProxies[i] ?? 0,
      polygon: octagonPolygon(pt.x, pt.y, r),
    };
  });

  // One representative token per cluster (artifact stores cluster summaries,
  // not per-vector positions).
  const tokens: TokenGeometry2d[] =
      clusters.map(cl => ({
                     tokenId: `cluster-${cl.clusterId}`,
                     x: cl.centroid.x,
                     y: cl.centroid.y,
                     clusterId: cl.clusterId,
                     weightedScore: cl.weightedScore,
                   }));

  // Artifact mode_flag: 0 = multiplicative, 1 = power. Use caller mode for
  // response since the caller selects which score to surface.
  const artifactMode: TrainingSessionMode =
      modeFlagRaw === 1 ? 'power' : 'multiplicative';
  void artifactMode;  // recorded above for future ABI cross-check

  return {
    contractVersion: 1,
    sessionId,
    epoch: epochIndex,
    mode,
    spectralActive: spectralActiveRaw !== 0,
    clusters,
    tokens,
    projectionMetadata: {
      method: 'pca',
      components: 2,
      explainedVariance,
      dimensionality,
    },
  };
}

type PersistedEpochOverlay = {
  readonly contractVersion: 1; readonly sessionId: string; readonly epochIndex: number; readonly mode: TrainingSessionMode; readonly confidence: number; readonly persistedAtUtc: string; readonly geometry: {
    readonly spectralActive: boolean; readonly clusters: ReadonlyArray<ClusterGeometry2d>; readonly tokens: ReadonlyArray<TokenGeometry2d>; readonly projectionMetadata:
                                                                                                                                                         ProjectionMetadata;
  };
};

function buildGeometryFromSnapshot(
    sessionId: string,
    epochIndex: number,
    mode: TrainingSessionMode,
    analytics: EpochPersistenceAnalyticsSnapshot,
    ): EpochGeometry {
  const weightedByCluster = new Map < number, {
    sum: number;
    count: number
  }
  >();
  for (const score of analytics.k3h4.weightedVoronoiScores) {
    const current =
        weightedByCluster.get(score.clusterId) ?? {sum: 0, count: 0};
    weightedByCluster.set(score.clusterId, {
      sum: current.sum + q16ToFloat(score.weightedScoreQ16),
      count: current.count + 1,
    });
  }

  const radiusByCluster = new Map<number, number>();
  for (const radius of analytics.k3h4.radii) {
    radiusByCluster.set(
        radius.clusterId, q16ToFloat(radius.inscribedRadiusQ16));
  }

  const spectralByCluster = new Map<number, number>();
  for (const spectral of analytics.k3h4.spectralProxy) {
    spectralByCluster.set(
        spectral.clusterId, q16ToFloat(spectral.amplitudeProxyQ16));
  }

  const clusters: ClusterGeometry2d[] = analytics.k3h4.centers.map((center) => {
    const centroid =
        analytics.k3h4Projection.nodes[center.clusterId] ?? {x: 0, y: 0};
    const radius = radiusByCluster.get(center.clusterId) ?? 0;
    const scoreStats = weightedByCluster.get(center.clusterId);
    const weightedScore = scoreStats && scoreStats.count > 0 ?
        scoreStats.sum / scoreStats.count :
        0;
    const spectralProxy = spectralByCluster.get(center.clusterId) ?? 0;

    return {
      clusterId: center.clusterId,
      centroid,
      inscribedRadius: radius,
      weightedScore,
      spectralProxy,
      polygon: octagonPolygon(centroid.x, centroid.y, radius),
    };
  });

  const tokens: TokenGeometry2d[] =
      clusters.map((cluster) => ({
                     tokenId: `cluster-${cluster.clusterId}`,
                     x: cluster.centroid.x,
                     y: cluster.centroid.y,
                     clusterId: cluster.clusterId,
                     weightedScore: cluster.weightedScore,
                   }));

  return {
    contractVersion: 1,
    sessionId,
    epoch: epochIndex,
    mode,
    spectralActive: analytics.k3h4Runtime.spectralActivation !== 'disabled',
    clusters,
    tokens,
    projectionMetadata: {
      method: 'pca',
      components: 2,
      explainedVariance: clampUnit(analytics.k3h4Projection.alignment / 100),
      dimensionality: 3,
    },
  };
}

async function readEpochOverlay(
    sessionDir: string,
    epochIndex: number,
    ): Promise<PersistedEpochOverlay|null> {
  const overlayPath = path.join(sessionDir, `epoch-${epochIndex}.overlay.json`);
  try {
    const raw = await fs.readFile(overlayPath, 'utf8');
    return JSON.parse(raw) as PersistedEpochOverlay;
  } catch (error) {
    const code = (error as NodeJS.ErrnoException).code;
    if (code === 'ENOENT') {
      return null;
    }
    throw new K3h4VizServiceError(
        'training_unavailable',
        `Failed to read persisted epoch overlay: ${
            error instanceof Error ? error.message : String(error)}`,
        503,
    );
  }
}

async function listEpochOverlays(sessionDir: string):
    Promise<PersistedEpochOverlay[]> {
  let entries: string[];
  try {
    entries = await fs.readdir(sessionDir);
  } catch {
    return [];
  }

  const overlays: PersistedEpochOverlay[] = [];
  for (const entry of entries) {
    const match = /^epoch-(\d+)\.overlay\.json$/.exec(entry);
    if (!match) {
      continue;
    }
    const epochIndex = Number(match[1]);
    const overlay = await readEpochOverlay(sessionDir, epochIndex);
    if (overlay) {
      overlays.push(overlay);
    }
  }
  overlays.sort((a, b) => a.epochIndex - b.epochIndex);
  return overlays;
}

export function createFileSystemK3h4TrainingService(): K3h4TrainingService {
  const artifactRoot = resolveTrainingArtifactRoot();

  return {
    async createTrainingSession(mode: TrainingSessionMode):
        Promise<TrainingSession> {
          const createdAtUtc = new Date().toISOString();
          const sessionId = createSessionId();
          const sessionDir = path.join(artifactRoot, sessionId);

          await fs.mkdir(sessionDir, {recursive: true});
          await fs.writeFile(
              path.join(sessionDir, 'session.json'),
              JSON.stringify({sessionId, mode, createdAtUtc}, null, 2),
              'utf8',
          );

          return {sessionId, mode, createdAtUtc};
        },

    async recordEpochFromAnalytics(
        sessionId: string,
        mode: TrainingSessionMode,
        confidence: number,
        analytics: EpochPersistenceAnalyticsSnapshot,
        ): Promise<RecordedEpochResult> {
      validateSessionId(sessionId);
      const sessionDir = path.join(artifactRoot, sessionId);

      try {
        await fs.access(sessionDir);
      } catch {
        throw new K3h4VizServiceError(
            'session_not_found',
            `Training session '${sessionId}' was not found.`,
            404,
        );
      }

      const overlayRecords = await listEpochOverlays(sessionDir);
      const nextEpochIndex = overlayRecords.length === 0 ?
          0 :
          overlayRecords[overlayRecords.length - 1]!.epochIndex + 1;

      const geometry =
          buildGeometryFromSnapshot(sessionId, nextEpochIndex, mode, analytics);
      const persistedAtUtc = new Date().toISOString();
      const record: PersistedEpochOverlay = {
        contractVersion: 1,
        sessionId,
        epochIndex: nextEpochIndex,
        mode,
        confidence: clampUnit(confidence),
        persistedAtUtc,
        geometry: {
          spectralActive: geometry.spectralActive,
          clusters: geometry.clusters,
          tokens: geometry.tokens,
          projectionMetadata: geometry.projectionMetadata,
        },
      };

      const outPath =
          path.join(sessionDir, `epoch-${nextEpochIndex}.overlay.json`);
      await fs.writeFile(outPath, JSON.stringify(record), 'utf8');

      return {epochIndex: nextEpochIndex, persistedAtUtc};
    },

    async readConfidenceTimeSeries(
        sessionId: string,
        mode: TrainingSessionMode,
        ): Promise<ConfidenceTimeSeries> {
      validateSessionId(sessionId);
      const sessionDir = path.join(artifactRoot, sessionId);

      let entries: string[];
      try {
        entries = await fs.readdir(sessionDir);
      } catch (error) {
        if ((error as NodeJS.ErrnoException).code === 'ENOENT') {
          throw new K3h4VizServiceError(
              'session_not_found',
              `Training session '${sessionId}' was not found.`, 404);
        }
        throw new K3h4VizServiceError(
            'training_unavailable',
            `Failed to list training session artifacts: ${
                error instanceof Error ? error.message : String(error)}`,
            503,
        );
      }

      const epochFiles = entries
                             .map((entry) => {
                               const match = /^epoch-(\d+)\.bin$/.exec(entry);
                               if (!match) return null;
                               return {
                                 epochIndex: Number(match[1]),
                                 fileName: entry,
                               };
                             })
                             .filter(
                                 (entry):
                                     entry is {
                                       epochIndex: number;
                                       fileName: string;
                                     } => entry !== null)
                             .sort((a, b) => a.epochIndex - b.epochIndex);

      const epochs: EpochConfidence[] = [];

      for (const epoch of epochFiles) {
        const artifactPath = path.join(sessionDir, epoch.fileName);
        try {
          const artifact = Buffer.from(await fs.readFile(artifactPath));
          const confidence = decodeArtifactConfidence(artifact, mode);
          epochs.push({
            epochIndex: epoch.epochIndex,
            confidence,
            mode,
          });
        } catch (error) {
          if (error instanceof K3h4VizServiceError &&
              error.code === 'artifact_incomplete') {
            // Partial artifacts are omitted from the confidence series.
            continue;
          }
          throw error;
        }
      }

      const overlays = await listEpochOverlays(sessionDir);
      for (const overlay of overlays) {
        epochs.push({
          epochIndex: overlay.epochIndex,
          confidence: overlay.confidence,
          mode,
        });
      }

      const dedupedEpochs =
          Array
              .from(new Map(epochs.map((epoch) => [epoch.epochIndex, epoch]))
                        .values())
              .sort((a, b) => a.epochIndex - b.epochIndex);

      const status: SessionStatus =
          dedupedEpochs.length === 0 ? 'pending' : 'active';
      const confidences = dedupedEpochs.map((epoch) => epoch.confidence);
      const peakEpoch = dedupedEpochs.length === 0 ?
          null :
          dedupedEpochs
              .reduce(
                  (best, current) =>
                      current.confidence > best.confidence ? current : best)
              .epochIndex;

      return {
        contractVersion: 1,
        sessionId,
        status,
        mode,
        epochs: dedupedEpochs,
        metadata: {
          peakEpoch,
          rollingAverage3: computeRollingAverage3(confidences),
          defaultMode: 'power',
        },
      };
    },

    async readEpochGeometry(
        sessionId: string,
        epochIndex: number,
        mode: TrainingSessionMode,
        ): Promise<EpochGeometry> {
      validateSessionId(sessionId);
      if (!Number.isInteger(epochIndex) || epochIndex < 0) {
        throw new K3h4VizServiceError(
            'invalid_epoch', 'Epoch index must be a non-negative integer.',
            400);
      }

      const sessionDir = path.join(artifactRoot, sessionId);
      const artifactPath = path.join(sessionDir, `epoch-${epochIndex}.bin`);

      let rawBuffer: Buffer;
      try {
        rawBuffer = Buffer.from(await fs.readFile(artifactPath));
      } catch (error) {
        const code = (error as NodeJS.ErrnoException).code;
        if (code === 'ENOENT') {
          // Check whether the session directory itself exists to distinguish
          // "session not found" from "epoch not yet written".
          try {
            await fs.access(sessionDir);
          } catch {
            throw new K3h4VizServiceError(
                'session_not_found',
                `Training session '${sessionId}' was not found.`, 404);
          }

          const overlay = await readEpochOverlay(sessionDir, epochIndex);
          if (overlay) {
            return {
              contractVersion: 1,
              sessionId,
              epoch: overlay.epochIndex,
              mode,
              spectralActive: overlay.geometry.spectralActive,
              clusters: overlay.geometry.clusters,
              tokens: overlay.geometry.tokens,
              projectionMetadata: overlay.geometry.projectionMetadata,
            };
          }

          throw new K3h4VizServiceError(
              'artifact_not_found',
              `Epoch ${epochIndex} artifact not found for session '${
                  sessionId}'.`,
              404,
          );
        }
        throw new K3h4VizServiceError(
            'training_unavailable',
            `Failed to read epoch artifact: ${
                error instanceof Error ? error.message : String(error)}`,
            503,
        );
      }

      return decodeArtifactGeometry(rawBuffer, sessionId, epochIndex, mode);
    },
  };
}
