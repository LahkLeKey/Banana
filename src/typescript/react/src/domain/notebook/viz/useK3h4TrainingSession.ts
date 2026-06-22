import {useCallback, useEffect, useRef, useState} from 'react';

import {createK3h4TrainingSession, fetchK3h4ConfidenceTimeSeriesBulk, fetchK3h4EpochGeometryBulk, fetchNetcodeAnalytics, type K3h4ConfidenceTimeSeries, type K3h4EpochConfidence, type K3h4TrainingMode, type K3h4TrainingSession, NetcodeAnalyticsError, type NetcodeAnalyticsRequest, type NetcodeAnalyticsResponse, recordK3h4TrainingEpoch, resolveApiBaseUrl,} from '../../../lib/api';
import type {K3h4EpochGeometryResponse} from '../../../lib/k3h4GeometryTypes';

const STORAGE_KEY = 'banana-k3h4-training-session';
const POLL_INTERVAL_MS = typeof import.meta.env !== 'undefined' &&
        import.meta.env.VITE_K3H4_CONFIDENCE_POLL_MS ?
    Number(import.meta.env.VITE_K3H4_CONFIDENCE_POLL_MS) :
    4000;

function readStoredSession(): K3h4TrainingSession|null {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) return null;
    return JSON.parse(raw) as K3h4TrainingSession;
  } catch {
    return null;
  }
}

function writeStoredSession(session: K3h4TrainingSession|null): void {
  try {
    if (session) {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(session));
    } else {
      localStorage.removeItem(STORAGE_KEY);
    }
  } catch {
    // localStorage may be unavailable (SSR / privacy mode).
  }
}

export type TrainingSessionPhase = 'idle'|'creating'|'active'|'error';
export type K3h4TrainingWorkflow = 'bootstrap'|'relations'|'churn';
export type TrainingWorkflowLogLevel = 'info'|'success'|'warn'|'error';

export type TrainingWorkflowLogEntry = {
  readonly id: string; readonly timestamp: string; readonly level: TrainingWorkflowLogLevel; readonly message:
                                                                                                          string;
  readonly details?: string;
};

export type TrainingWorkflowState = {
  readonly status: 'idle'|'running'|'completed'|'error'; readonly workflow: K3h4TrainingWorkflow | null; readonly totalPasses: number; readonly completedPasses: number; readonly errorMessage:
                                                                                                                                                                                      string |
      null;
};

const WORKFLOW_PASS_COUNT: Record<K3h4TrainingWorkflow, number> = {
  bootstrap: 12,
  relations: 36,
  churn: 96,
};

const RATE_LIMIT_MAX_RETRIES_PER_PASS = 6;
const RATE_LIMIT_BACKOFF_BASE_MS = 1200;
const RATE_LIMIT_BACKOFF_MAX_MS = 20000;

function delay(ms: number): Promise<void> {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

function parseRetryAfterMs(message: string): number|null {
  const retryAfterMatch =
      /retry\s*after\s*(\d+(?:\.\d+)?)\s*(ms|s|sec|secs|second|seconds|m|min|mins|minute|minutes)?/i
          .exec(message);
  if (!retryAfterMatch) {
    return null;
  }

  const value = Number(retryAfterMatch[1]);
  if (!Number.isFinite(value) || value <= 0) {
    return null;
  }

  const unit = (retryAfterMatch[2] ?? 's').toLowerCase();
  if (unit === 'ms') {
    return Math.round(value);
  }
  if (unit.startsWith('m')) {
    return Math.round(value * 60000);
  }
  return Math.round(value * 1000);
}

function computeRateLimitBackoffMs(attemptIndex: number): number {
  const exponential = Math.min(
      RATE_LIMIT_BACKOFF_MAX_MS,
      RATE_LIMIT_BACKOFF_BASE_MS * (2 ** Math.max(0, attemptIndex)),
  );
  const jitter = Math.floor(Math.random() * 350);
  return Math.min(RATE_LIMIT_BACKOFF_MAX_MS, exponential + jitter);
}

function resolveRateLimitWaitMs(
    error: unknown,
    attemptIndex: number,
    ): number|null {
  if (error instanceof NetcodeAnalyticsError) {
    if (error.status !== 429) {
      return null;
    }

    const payloadMessage = typeof error.payload.message === 'string' ?
        error.payload.message :
        typeof error.payload.error === 'string' ? error.payload.error :
                                                  '';
    const fromPayload = parseRetryAfterMs(payloadMessage);
    if (fromPayload !== null) {
      return fromPayload;
    }
    const fromMessage = parseRetryAfterMs(error.message);
    if (fromMessage !== null) {
      return fromMessage;
    }

    return computeRateLimitBackoffMs(attemptIndex);
  }

  const message = extractErrorMessage(error);
  if (!/rate\s*limit|too\s*many\s*requests|throttl/i.test(message)) {
    return null;
  }

  const hintedWait = parseRetryAfterMs(message);
  if (hintedWait !== null) {
    return hintedWait;
  }

  return computeRateLimitBackoffMs(attemptIndex);
}

function clampUnit(value: number): number {
  if (!Number.isFinite(value)) {
    return 0;
  }
  return Math.min(1, Math.max(0, value));
}

function clampSigned(value: number, magnitude: number): number {
  if (!Number.isFinite(value)) {
    return 0;
  }
  return Math.min(magnitude, Math.max(-magnitude, value));
}

type K3h4FeatureVector = {
  readonly scoreSignal: number; readonly stabilitySignal: number; readonly spectralSignal: number; readonly convergenceSignal: number; readonly clusterCount:
                                                                                                                                                    number;
};

type K3h4AdaptiveState = {
  readonly modelConfidenceBias: number; readonly policyMomentumBias: number; readonly callDensityBias: number; readonly dependencyPulseBias: number; readonly interactionSignalBias: number; readonly stagnantPasses: number; readonly previousFeature:
                                                                                                                                                                                                                                           K3h4FeatureVector |
      null;
};

function createInitialAdaptiveState(): K3h4AdaptiveState {
  return {
    modelConfidenceBias: 0,
    policyMomentumBias: 0,
    callDensityBias: 0,
    dependencyPulseBias: 0,
    interactionSignalBias: 0,
    stagnantPasses: 0,
    previousFeature: null,
  };
}

function extractK3h4FeatureVector(analytics: NetcodeAnalyticsResponse):
    K3h4FeatureVector {
  const weighted = analytics.k3h4.weightedVoronoiScores;
  const radii = analytics.k3h4.radii;
  const spectral = analytics.k3h4.spectralProxy;

  const validScoreCount =
      weighted.filter((score) => score.scoreValidity === 'valid').length;
  const validScoreRatio =
      weighted.length > 0 ? validScoreCount / weighted.length : 0;

  const stableRadiusCount =
      radii.filter((radius) => radius.radiusState === 'ok').length;
  const stableRadiusRatio =
      radii.length > 0 ? stableRadiusCount / radii.length : 0;

  const spectralStrength = spectral.length > 0 ?
      spectral.reduce(
          (sum, entry) => sum + Math.abs(entry.frequencyProxyQ16) +
              Math.abs(entry.amplitudeProxyQ16),
          0,
          ) /
          (spectral.length * 131072) :
      0;

  const alignmentSignal = clampUnit(analytics.k3h4Projection.alignment / 100);
  const radialStabilitySignal =
      clampUnit(analytics.k3h4Projection.radialStability / 100);
  const scoreSignal =
      clampUnit(validScoreRatio * 0.55 + alignmentSignal * 0.45);
  const stabilitySignal =
      clampUnit(stableRadiusRatio * 0.5 + radialStabilitySignal * 0.5);

  const convergenceSignal =
      analytics.k3h4.observability.convergenceStatus === 'converged' ? 1 :
      analytics.k3h4.observability.convergenceStatus === 'max-iterations' ?
                                                                       0.5 :
                                                                       0;

  return {
    scoreSignal,
    stabilitySignal,
    spectralSignal: clampUnit(spectralStrength),
    convergenceSignal,
    clusterCount: analytics.k3h4.centers.length,
  };
}

function evolveAdaptiveState(
    previous: K3h4AdaptiveState,
    nextFeature: K3h4FeatureVector,
    ): K3h4AdaptiveState {
  const previousComposite = previous.previousFeature ?
      previous.previousFeature.scoreSignal * 0.45 +
          previous.previousFeature.stabilitySignal * 0.35 +
          previous.previousFeature.convergenceSignal * 0.20 :
      nextFeature.scoreSignal * 0.45 + nextFeature.stabilitySignal * 0.35 +
          nextFeature.convergenceSignal * 0.20;
  const nextComposite = nextFeature.scoreSignal * 0.45 +
      nextFeature.stabilitySignal * 0.35 + nextFeature.convergenceSignal * 0.20;
  const delta = nextComposite - previousComposite;

  const stagnantPasses =
      Math.abs(delta) < 0.01 ? previous.stagnantPasses + 1 : 0;
  const plateauBoost = Math.min(1, stagnantPasses / 4) * 0.03;

  const directionalAdjust = clampSigned(delta * 0.35, 0.04);
  const scoreTargetBias =
      clampSigned((nextFeature.scoreSignal - 0.62) * 0.20, 0.05);
  const stabilityTargetBias =
      clampSigned((nextFeature.stabilitySignal - 0.58) * 0.18, 0.05);
  const spectralTargetBias =
      clampSigned((nextFeature.spectralSignal - 0.52) * 0.12, 0.04);

  return {
    modelConfidenceBias: clampSigned(
        previous.modelConfidenceBias + directionalAdjust + scoreTargetBias -
            plateauBoost,
        0.12,
        ),
    policyMomentumBias: clampSigned(
        previous.policyMomentumBias + directionalAdjust * 0.75 +
            stabilityTargetBias,
        0.12,
        ),
    callDensityBias: clampSigned(
        previous.callDensityBias + plateauBoost + spectralTargetBias,
        0.10,
        ),
    dependencyPulseBias: clampSigned(
        previous.dependencyPulseBias + plateauBoost * 0.7 +
            (nextFeature.clusterCount >= 4 ? 0.01 : -0.005),
        0.10,
        ),
    interactionSignalBias: clampSigned(
        previous.interactionSignalBias + directionalAdjust * 0.5 +
            spectralTargetBias,
        0.08,
        ),
    stagnantPasses,
    previousFeature: nextFeature,
  };
}

function extractErrorMessage(error: unknown): string {
  if (typeof error === 'object' && error !== null) {
    const record = error as Record<string, unknown>;
    const nestedCandidates = [
      record.payload,
      record.cause,
      record.response,
      record.data,
      record.body,
      record.message,
      record.error,
      record.detail,
      record.details,
      record.reason,
    ];

    for (const candidate of nestedCandidates) {
      if (typeof candidate === 'string' && candidate.trim().length > 0) {
        if (candidate === '[object Object]') {
          continue;
        }
        return candidate;
      }
      if (typeof candidate === 'object' && candidate !== null) {
        const nested = extractErrorMessage(candidate);
        if (nested !== 'Unknown error' && nested !== '[object Object]') {
          return nested;
        }
      }
    }

    if (error instanceof Error && error.message.trim().length > 0 &&
        error.message !== '[object Object]') {
      return error.message;
    }

    try {
      return JSON.stringify(error);
    } catch {
      return 'Unknown error';
    }
  }

  if (typeof error === 'string' && error.trim().length > 0) {
    return error;
  }

  if (error instanceof Error && error.message.trim().length > 0) {
    return error.message;
  }

  return 'Unknown error';
}

function isSessionNotFoundMessage(message: string): boolean {
  const normalized = message.toLowerCase();
  return normalized.includes('session_not_found') ||
      normalized.includes('training session not found') ||
      normalized.includes('session not found');
}

function computeEpochConfidenceFromFeature(feature: K3h4FeatureVector): number {
  const blended = feature.scoreSignal * 0.46 + feature.stabilitySignal * 0.34 +
      feature.convergenceSignal * 0.20;
  return clampUnit(blended);
}

function deriveMetadataFromEpochs(epochs: readonly K3h4EpochConfidence[]): {
  readonly peakEpoch: number|null; readonly rollingAverage3: number | null; readonly defaultMode:
                                                                                         'power';
} {
  if (epochs.length === 0) {
    return {peakEpoch: null, rollingAverage3: null, defaultMode: 'power'};
  }

  const peak = epochs.reduce(
      (best, current) => current.confidence > best.confidence ? current : best,
      epochs[0]!,
  );
  const rollingAverage3 = epochs.length >= 3 ?
      Number(
          (epochs[epochs.length - 1]!.confidence +
           epochs[epochs.length - 2]!.confidence +
           epochs[epochs.length - 3]!.confidence) /
          3) :
      null;

  return {
    peakEpoch: peak.epochIndex,
    rollingAverage3,
    defaultMode: 'power',
  };
}

function buildAnalyticsPayload(params: {
  workflow: K3h4TrainingWorkflow; mode: K3h4TrainingMode; passIndex: number;
  adaptive: K3h4AdaptiveState;
}): NetcodeAnalyticsRequest {
  const {workflow, mode, passIndex, adaptive} = params;
  const wave = Math.sin(passIndex / 3);
  const sweep = Math.cos(passIndex / 5);
  const workflowDepth = workflow === 'bootstrap' ? 1 :
      workflow === 'relations'                   ? 2 :
                                                   3;

  return {
    callDensity: clampUnit(0.42 + (wave + 1) * 0.14 + adaptive.callDensityBias),
    questPercent: 0.38 + (sweep + 1) * 0.18,
    playerLevel: 7 + (passIndex % 18),
    comboStreak: 2 + (passIndex % 9),
    branchPressure: 0.32 + (Math.abs(wave) * 0.36),
    dependencyPulse: clampUnit(
        0.34 + (Math.abs(sweep) * 0.34) + adaptive.dependencyPulseBias),
    workflowDepth,
    networkDimensions: workflow === 'churn' ? 4 : 3,
    modelConfidence: clampUnit(
        0.35 + (Math.abs(wave) * 0.55) + adaptive.modelConfidenceBias),
    policyMomentum: clampUnit(
        0.33 + (Math.abs(sweep) * 0.58) + adaptive.policyMomentumBias),
    interactionSignal: clampUnit(
        0.44 + ((wave + sweep + 2) / 4) * 0.45 +
        adaptive.interactionSignalBias),
    k3h4Mode: mode,
    spectralMode: workflow === 'relations' || workflow === 'churn' ?
        'affinity-graph' :
        'disabled',
  };
}

export type UseK3h4TrainingSessionResult = {
  phase: TrainingSessionPhase; session: K3h4TrainingSession | null;
  confidence: K3h4ConfidenceTimeSeries | null;
  latestGeometry: K3h4EpochGeometryResponse | null;
  errorMessage: string | null;
  workflowState: TrainingWorkflowState;
  workflowLogs: readonly TrainingWorkflowLogEntry[];
  startSession: (mode: K3h4TrainingMode) => void;
  runWorkflow: (workflow: K3h4TrainingWorkflow) => void;
  orchestrateRealtimeTraining:
      (mode: K3h4TrainingMode, workflow?: K3h4TrainingWorkflow) => void;
  stopWorkflow: () => void;
  clearWorkflowLogs: () => void;
  clearSession: () => void;
};

export function useK3h4TrainingSession(): UseK3h4TrainingSessionResult {
  const [phase, setPhase] = useState<TrainingSessionPhase>('idle');
  const [session, setSession] =
      useState<K3h4TrainingSession|null>(() => readStoredSession());
  const [confidence, setConfidence] =
      useState<K3h4ConfidenceTimeSeries|null>(null);
  const [latestGeometry, setLatestGeometry] =
      useState<K3h4EpochGeometryResponse|null>(null);
  const [errorMessage, setErrorMessage] = useState<string|null>(null);
  const [workflowState, setWorkflowState] = useState<TrainingWorkflowState>({
    status: 'idle',
    workflow: null,
    totalPasses: 0,
    completedPasses: 0,
    errorMessage: null,
  });
  const [workflowLogs, setWorkflowLogs] =
      useState<TrainingWorkflowLogEntry[]>([]);

  const pollRef = useRef<ReturnType<typeof setInterval>|null>(null);
  const workflowRunIdRef = useRef(0);
  const sessionRef = useRef<K3h4TrainingSession|null>(session);

  useEffect(() => {
    sessionRef.current = session;
  }, [session]);

  const appendWorkflowLog = useCallback(
      (entry: {
        level: TrainingWorkflowLogLevel; message: string;
        details?: string;
      }) => {
        setWorkflowLogs((current) => {
          const next: TrainingWorkflowLogEntry = {
            id: `${Date.now()}-${Math.random().toString(36).slice(2, 10)}`,
            timestamp: new Date().toISOString(),
            level: entry.level,
            message: entry.message,
            details: entry.details,
          };
          const updated = [...current, next];
          return updated.length > 400 ? updated.slice(updated.length - 400) :
                                        updated;
        });
      },
      []);

  // When a session is already in storage, resume polling immediately.
  useEffect(() => {
    if (session) setPhase('active');
  }, []);  // eslint-disable-line react-hooks/exhaustive-deps

  const stopPolling = useCallback(() => {
    if (pollRef.current !== null) {
      clearInterval(pollRef.current);
      pollRef.current = null;
    }
  }, []);

  const pollConfidence = useCallback(
      async (activeSession: K3h4TrainingSession) => {
        const baseUrl = resolveApiBaseUrl();
        try {
          // Single bulk request covers both confidence and latest-epoch
          // geometry so one poll tick costs 1 API call regardless of how many
          // sessions the panel is tracking — keeping workloads below the prod
          // rate limit.
          const bulkResult = await fetchK3h4ConfidenceTimeSeriesBulk(
              baseUrl, [activeSession.sessionId], activeSession.mode);

          const sessionResult = bulkResult.results[0];
          if (!sessionResult || sessionResult.error) {
            const errorCode = sessionResult?.error?.code ?? 'unknown';
            const errorMsg =
                sessionResult?.error?.message ?? 'Confidence fetch failed';
            if (isSessionNotFoundMessage(errorCode) ||
                isSessionNotFoundMessage(errorMsg) ||
                sessionResult?.error?.statusCode === 404) {
              writeStoredSession(null);
              setSession(null);
              setConfidence(null);
              setLatestGeometry(null);
              setPhase('idle');
              setErrorMessage(
                  'Training session expired on server. Start a new session and rerun workflow.',
              );
              stopPolling();
            }
            return;
          }

          const series = sessionResult.confidence!;
          setConfidence(series);

          if (series.status === 'completed') stopPolling();

          // Fetch geometry for the latest epoch in the same bulk call.
          if (series.epochs.length > 0) {
            const lastEpoch =
                series.epochs[series.epochs.length - 1]!.epochIndex;
            try {
              const geoBulk = await fetchK3h4EpochGeometryBulk(
                  baseUrl,
                  [{sessionId: activeSession.sessionId, epochIndex: lastEpoch}],
                  activeSession.mode,
              );
              const geoResult = geoBulk.results[0];
              if (geoResult?.geometry) {
                setLatestGeometry(geoResult.geometry);
              }
            } catch {
              // Geometry is best-effort; don't break the confidence poll.
            }
          }
        } catch (err) {
          // If server no longer knows the session, clear stale local state.
          const msg = extractErrorMessage(err);
          if (isSessionNotFoundMessage(msg) || msg.includes('404')) {
            writeStoredSession(null);
            setSession(null);
            setConfidence(null);
            setLatestGeometry(null);
            setPhase('idle');
            setErrorMessage(
                'Training session expired on server. Start a new session and rerun workflow.',
            );
            stopPolling();
          }
          // Other transient errors are swallowed; next tick will retry.
        }
      },
      [stopPolling],
  );

  const startPolling = useCallback(
      (activeSession: K3h4TrainingSession) => {
        stopPolling();
        void pollConfidence(activeSession);
        pollRef.current = setInterval(
            () => void pollConfidence(activeSession), POLL_INTERVAL_MS);
      },
      [pollConfidence, stopPolling],
  );

  // Resume polling whenever phase becomes active.
  useEffect(() => {
    if (phase === 'active' && session) startPolling(session);
    return stopPolling;
  }, [phase, session, startPolling, stopPolling]);

  const startSession = useCallback((mode: K3h4TrainingMode) => {
    setPhase('creating');
    setErrorMessage(null);
    const baseUrl = resolveApiBaseUrl();
    createK3h4TrainingSession(baseUrl, mode)
        .then((created) => {
          writeStoredSession(created);
          setSession(created);
          setConfidence(null);
          setLatestGeometry(null);
          setPhase('active');
          appendWorkflowLog({
            level: 'success',
            message: `Session ${created.sessionId} created (${created.mode})`,
          });
        })
        .catch((err) => {
          const msg = extractErrorMessage(err);
          setErrorMessage(msg);
          setPhase('error');
          appendWorkflowLog({
            level: 'error',
            message: 'Failed to create training session',
            details: msg,
          });
        });
  }, [appendWorkflowLog]);

  const stopWorkflow = useCallback(() => {
    workflowRunIdRef.current += 1;
    setWorkflowState(
        (current) => ({
          status: current.status === 'running' ? 'idle' : current.status,
          workflow: current.status === 'running' ? null : current.workflow,
          totalPasses: current.status === 'running' ? 0 : current.totalPasses,
          completedPasses:
              current.status === 'running' ? 0 : current.completedPasses,
          errorMessage: null,
        }));
    appendWorkflowLog({
      level: 'warn',
      message: 'Workflow run stopped by user',
    });
  }, [appendWorkflowLog]);

  const executeWorkflowForSession = useCallback(
      (activeSession: K3h4TrainingSession, workflow: K3h4TrainingWorkflow) => {
        const runId = workflowRunIdRef.current + 1;
        workflowRunIdRef.current = runId;

        const totalPasses = WORKFLOW_PASS_COUNT[workflow];
        setWorkflowState({
          status: 'running',
          workflow,
          totalPasses,
          completedPasses: 0,
          errorMessage: null,
        });
        appendWorkflowLog({
          level: 'info',
          message: `Starting ${workflow} workflow (${totalPasses} passes)`,
          details:
              `mode=${activeSession.mode} session=${activeSession.sessionId}`,
        });

        const baseUrl = resolveApiBaseUrl();
        if (!baseUrl) {
          setWorkflowState({
            status: 'error',
            workflow,
            totalPasses,
            completedPasses: 0,
            errorMessage: 'API base URL is not configured.',
          });
          appendWorkflowLog({
            level: 'error',
            message: 'Workflow failed: API base URL is missing',
          });
          return;
        }

        void (async () => {
          let adaptiveState = createInitialAdaptiveState();

          for (let passIndex = 0; passIndex < totalPasses; passIndex += 1) {
            if (workflowRunIdRef.current !== runId) {
              return;
            }

            const payload = buildAnalyticsPayload({
              workflow,
              mode: activeSession.mode,
              passIndex,
              adaptive: adaptiveState,
            });

            let completedPass = false;
            let rateLimitRetries = 0;

            while (!completedPass) {
              if (workflowRunIdRef.current !== runId) {
                return;
              }

              try {
                const analytics = await fetchNetcodeAnalytics(baseUrl, payload);
                const featureVector = extractK3h4FeatureVector(analytics);
                adaptiveState =
                    evolveAdaptiveState(adaptiveState, featureVector);

                appendWorkflowLog({
                  level: 'info',
                  message: `Pass ${passIndex + 1}/${totalPasses} complete`,
                  details: `depth=${payload.workflowDepth} density=${
                      payload.callDensity.toFixed(3)} confidence=${
                      payload.modelConfidence.toFixed(3)} clusters=${
                      analytics.k3h4.centers.length} convergence=${
                      analytics.k3h4.observability
                          .convergenceStatus} featureScore=${
                      featureVector.scoreSignal.toFixed(3)} stability=${
                      featureVector.stabilitySignal.toFixed(3)}`,
                });

                const derivedConfidence =
                    computeEpochConfidenceFromFeature(featureVector);

                const persisted = await recordK3h4TrainingEpoch(
                    baseUrl,
                    activeSession.sessionId,
                    {
                      mode: activeSession.mode,
                      confidence: derivedConfidence,
                      analytics: {
                        k3h4Projection: {
                          alignment: analytics.k3h4Projection.alignment,
                          radialStability:
                              analytics.k3h4Projection.radialStability,
                          nodes: analytics.k3h4Projection.nodes.map((node) => ({
                                                                      x: node.x,
                                                                      y: node.y,
                                                                    })),
                        },
                        k3h4: {
                          centers: analytics.k3h4.centers.map(
                              (center) => ({
                                clusterId: center.clusterId,
                              })),
                          radii: analytics.k3h4.radii.map(
                              (radius) => ({
                                clusterId: radius.clusterId,
                                inscribedRadiusQ16: radius.inscribedRadiusQ16,
                              })),
                          weightedVoronoiScores:
                              analytics.k3h4.weightedVoronoiScores.map(
                                  (score) => ({
                                    clusterId: score.clusterId,
                                    weightedScoreQ16: score.weightedScoreQ16,
                                  })),
                          spectralProxy: analytics.k3h4.spectralProxy.map(
                              (spectral) => ({
                                clusterId: spectral.clusterId,
                                amplitudeProxyQ16: spectral.amplitudeProxyQ16,
                              })),
                        },
                        k3h4Runtime: {
                          spectralActivation:
                              analytics.k3h4Runtime.spectralActivation,
                        },
                      },
                    },
                );

                setConfidence((existing) => {
                  const baseSeries: K3h4ConfidenceTimeSeries = existing ?? {
                    contractVersion: 1,
                    sessionId: activeSession.sessionId,
                    status: 'pending',
                    mode: activeSession.mode,
                    epochs: [],
                    metadata: {
                      peakEpoch: null,
                      rollingAverage3: null,
                      defaultMode: 'power',
                    },
                  };
                  const mergedByEpoch = new Map<number, K3h4EpochConfidence>();
                  for (const epoch of baseSeries.epochs) {
                    mergedByEpoch.set(epoch.epochIndex, epoch);
                  }
                  mergedByEpoch.set(persisted.epochIndex, {
                    epochIndex: persisted.epochIndex,
                    confidence: derivedConfidence,
                    mode: activeSession.mode,
                  });
                  const mergedEpochs =
                      Array.from(mergedByEpoch.values())
                          .sort((a, b) => a.epochIndex - b.epochIndex);
                  return {
                    ...baseSeries,
                    status: mergedEpochs.length > 0 ? 'active' :
                                                      baseSeries.status,
                    epochs: mergedEpochs,
                    metadata: deriveMetadataFromEpochs(mergedEpochs),
                  };
                });

                completedPass = true;
              } catch (error) {
                if (workflowRunIdRef.current !== runId) {
                  return;
                }

                const message = extractErrorMessage(error);
                if (isSessionNotFoundMessage(message)) {
                  writeStoredSession(null);
                  setSession(null);
                  setConfidence(null);
                  setLatestGeometry(null);
                  setPhase('idle');
                  setErrorMessage(
                      'Training session expired on server. Start a new session and rerun workflow.',
                  );
                  setWorkflowState({
                    status: 'error',
                    workflow,
                    totalPasses,
                    completedPasses: passIndex,
                    errorMessage:
                        'Training session expired on server. Start a new session and rerun workflow.',
                  });
                  appendWorkflowLog({
                    level: 'warn',
                    message:
                        'Workflow stopped: server session was not found; local session has been reset',
                    details: message,
                  });
                  return;
                }

                const waitMs = resolveRateLimitWaitMs(error, rateLimitRetries);
                if (waitMs !== null &&
                    rateLimitRetries < RATE_LIMIT_MAX_RETRIES_PER_PASS) {
                  rateLimitRetries += 1;
                  appendWorkflowLog({
                    level: 'warn',
                    message: `Rate limit detected at pass ${passIndex + 1}/${
                        totalPasses}; waiting before retry`,
                    details: `retry=${rateLimitRetries}/${
                        RATE_LIMIT_MAX_RETRIES_PER_PASS} waitMs=${waitMs}`,
                  });
                  await delay(waitMs);
                  continue;
                }

                setWorkflowState({
                  status: 'error',
                  workflow,
                  totalPasses,
                  completedPasses: passIndex,
                  errorMessage: message,
                });
                appendWorkflowLog({
                  level: 'error',
                  message:
                      `Workflow error at pass ${passIndex + 1}/${totalPasses}`,
                  details: message,
                });
                return;
              }
            }

            if (workflowRunIdRef.current !== runId) {
              return;
            }

            setWorkflowState({
              status: 'running',
              workflow,
              totalPasses,
              completedPasses: passIndex + 1,
              errorMessage: null,
            });

            await delay(120);
          }

          if (workflowRunIdRef.current !== runId) {
            return;
          }

          setWorkflowState({
            status: 'completed',
            workflow,
            totalPasses,
            completedPasses: totalPasses,
            errorMessage: null,
          });
          appendWorkflowLog({
            level: 'success',
            message: `Workflow ${workflow} completed`,
            details: `passes=${totalPasses} session=${activeSession.sessionId}`,
          });

          await pollConfidence(activeSession);
        })();
      },
      [appendWorkflowLog, pollConfidence]);

  const runWorkflow = useCallback((workflow: K3h4TrainingWorkflow) => {
    if (!session) {
      setWorkflowState({
        status: 'error',
        workflow: null,
        totalPasses: 0,
        completedPasses: 0,
        errorMessage: 'Start a session before running workflow actions.',
      });
      appendWorkflowLog({
        level: 'error',
        message: 'Workflow start rejected: no active session',
      });
      return;
    }

    executeWorkflowForSession(session, workflow);
  }, [appendWorkflowLog, executeWorkflowForSession, session]);

  const orchestrateRealtimeTraining = useCallback(
      (mode: K3h4TrainingMode,
       workflow: K3h4TrainingWorkflow = 'bootstrap') => {
        const existingSession = sessionRef.current;
        if (existingSession) {
          executeWorkflowForSession(existingSession, workflow);
          return;
        }

        setPhase('creating');
        setErrorMessage(null);
        const baseUrl = resolveApiBaseUrl();
        createK3h4TrainingSession(baseUrl, mode)
            .then((created) => {
              writeStoredSession(created);
              setSession(created);
              setConfidence(null);
              setLatestGeometry(null);
              setPhase('active');
              appendWorkflowLog({
                level: 'success',
                message:
                    `Session ${created.sessionId} created (${created.mode})`,
              });
              executeWorkflowForSession(created, workflow);
            })
            .catch((err) => {
              const msg = extractErrorMessage(err);
              setErrorMessage(msg);
              setPhase('error');
              appendWorkflowLog({
                level: 'error',
                message: 'Failed to create training session',
                details: msg,
              });
            });
      },
      [appendWorkflowLog, executeWorkflowForSession],
  );

  const clearWorkflowLogs = useCallback(() => {
    setWorkflowLogs([]);
  }, []);

  const clearSession = useCallback(() => {
    workflowRunIdRef.current += 1;
    stopPolling();
    writeStoredSession(null);
    setSession(null);
    setConfidence(null);
    setLatestGeometry(null);
    setErrorMessage(null);
    setWorkflowState({
      status: 'idle',
      workflow: null,
      totalPasses: 0,
      completedPasses: 0,
      errorMessage: null,
    });
    setWorkflowLogs([]);
    setPhase('idle');
  }, [stopPolling]);

  return {
    phase,
    session,
    confidence,
    latestGeometry,
    errorMessage,
    workflowState,
    workflowLogs,
    startSession,
    runWorkflow,
    orchestrateRealtimeTraining,
    stopWorkflow,
    clearWorkflowLogs,
    clearSession,
  };
}
