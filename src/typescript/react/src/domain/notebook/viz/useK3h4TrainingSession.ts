import {useCallback, useEffect, useRef, useState} from 'react';

import {createK3h4TrainingSession, fetchK3h4ConfidenceTimeSeries, fetchK3h4EpochGeometry, fetchNetcodeAnalytics, type K3h4ConfidenceTimeSeries, type K3h4TrainingMode, type K3h4TrainingSession, type NetcodeAnalyticsRequest, resolveApiBaseUrl,} from '../../../lib/api';
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

function delay(ms: number): Promise<void> {
  return new Promise((resolve) => setTimeout(resolve, ms));
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

function buildAnalyticsPayload(params: {
  workflow: K3h4TrainingWorkflow; mode: K3h4TrainingMode; passIndex: number;
}): NetcodeAnalyticsRequest {
  const {workflow, mode, passIndex} = params;
  const wave = Math.sin(passIndex / 3);
  const sweep = Math.cos(passIndex / 5);
  const workflowDepth = workflow === 'bootstrap' ? 1 :
      workflow === 'relations'                   ? 2 :
                                                   3;

  return {
    callDensity: 0.42 + (wave + 1) * 0.14,
    questPercent: 0.38 + (sweep + 1) * 0.18,
    playerLevel: 7 + (passIndex % 18),
    comboStreak: 2 + (passIndex % 9),
    branchPressure: 0.32 + (Math.abs(wave) * 0.36),
    dependencyPulse: 0.34 + (Math.abs(sweep) * 0.34),
    workflowDepth,
    networkDimensions: workflow === 'churn' ? 4 : 3,
    modelConfidence: 0.35 + (Math.abs(wave) * 0.55),
    policyMomentum: 0.33 + (Math.abs(sweep) * 0.58),
    interactionSignal: 0.44 + ((wave + sweep + 2) / 4) * 0.45,
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
          const series = await fetchK3h4ConfidenceTimeSeries(
              baseUrl, activeSession.sessionId, activeSession.mode);
          setConfidence(series);

          if (series.status === 'completed') stopPolling();

          // Load geometry for the latest epoch when available.
          if (series.epochs.length > 0) {
            const lastEpoch =
                series.epochs[series.epochs.length - 1]!.epochIndex;
            try {
              const geo = await fetchK3h4EpochGeometry(
                  baseUrl, activeSession.sessionId, lastEpoch,
                  activeSession.mode);
              setLatestGeometry(geo);
            } catch {
              // Geometry is best-effort; don't break the confidence poll.
            }
          }
        } catch (err) {
          // 404 means session was pruned server-side — surface but keep going.
          const msg = extractErrorMessage(err);
          if (msg.includes('session_not_found') || msg.includes('404')) {
            setErrorMessage('Training session not found on server.');
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
      details: `mode=${session.mode} session=${session.sessionId}`,
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
      for (let passIndex = 0; passIndex < totalPasses; passIndex += 1) {
        if (workflowRunIdRef.current !== runId) {
          return;
        }

        const payload = buildAnalyticsPayload({
          workflow,
          mode: session.mode,
          passIndex,
        });

        try {
          const analytics = await fetchNetcodeAnalytics(baseUrl, payload);
          appendWorkflowLog({
            level: 'info',
            message: `Pass ${passIndex + 1}/${totalPasses} complete`,
            details: `depth=${payload.workflowDepth} density=${
                payload.callDensity.toFixed(3)} confidence=${
                payload.modelConfidence.toFixed(
                    3)} clusters=${analytics.k3h4.centers.length} convergence=${
                analytics.k3h4.observability.convergenceStatus}`,
          });
        } catch (error) {
          if (workflowRunIdRef.current !== runId) {
            return;
          }
          const message = extractErrorMessage(error);
          setWorkflowState({
            status: 'error',
            workflow,
            totalPasses,
            completedPasses: passIndex,
            errorMessage: message,
          });
          appendWorkflowLog({
            level: 'error',
            message: `Workflow error at pass ${passIndex + 1}/${totalPasses}`,
            details: message,
          });
          return;
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
        details: `passes=${totalPasses} session=${session.sessionId}`,
      });

      await pollConfidence(session);
    })();
  }, [appendWorkflowLog, pollConfidence, session]);

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
    stopWorkflow,
    clearWorkflowLogs,
    clearSession,
  };
}
