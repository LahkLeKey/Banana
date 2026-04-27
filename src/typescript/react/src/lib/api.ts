import type {ChatMessage, ChatSession, EnsembleVerdict, Ripeness, TrainingAuditEvent, TrainingLane, TrainingRunRequest, TrainingRunResult} from '@banana/ui';

type ErrorPayload = {
  error?: {message?: string;};
};

export type CreateSessionResponse = {
  session: ChatSession; welcome_message: ChatMessage;
};

export type SendMessageResponse = {
  session_id: string; duplicate: boolean; user_message: ChatMessage;
  assistant_message: ChatMessage;
};

export type RipenessResponse = {
  label: Ripeness; confidence: number;
};

export type TrainingRunResponse = {
  run: TrainingRunResult;
};

export type TrainingHistoryResponse = {
  count: number; rows: TrainingRunResult[]; audit: TrainingAuditEvent[];
};

export type TrainingRunDetailResponse = {
  run: TrainingRunResult; audit: TrainingAuditEvent[];
};

export type PromoteRunResponse = {
  promoted: {
    run_id: string; target: 'candidate' | 'stable'; threshold_passed: boolean;
  };
  audit_event: TrainingAuditEvent;
};

type BananaSummaryResponse = {
  banana: number;
};

type ElectronBridge = {
  apiBaseUrl: string; platform: string;
};

function resolveElectronBridge(): ElectronBridge|undefined {
  return typeof window !== 'undefined' ? window.banana : undefined;
}

function resolveViteApiBaseUrl(): string {
  const importMeta = import.meta as {env?: {VITE_BANANA_API_BASE_URL?: string}};
  return importMeta.env?.VITE_BANANA_API_BASE_URL ?? '';
}

export function resolveApiBaseResolutionError(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge()): string|null {
  if (viteBaseUrl.trim().length > 0) {
    return null;
  }

  if ((electronBridge?.apiBaseUrl ?? '').trim().length > 0) {
    return null;
  }

  return 'Missing API base URL. Set VITE_BANANA_API_BASE_URL for web or BANANA_API_BASE_URL for Electron preload, then relaunch via the canonical Compose frontend profile.';
}

export function resolveApiBaseUrl(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge()): string {
  return viteBaseUrl.trim() || electronBridge?.apiBaseUrl?.trim() || '';
}

export function resolvePlatformLabel(electronBridge = resolveElectronBridge()):
    string {
  return electronBridge?.platform ? `electron-${electronBridge.platform}` :
                                    'web';
}

async function parseApiError(response: Response): Promise<string> {
  try {
    const payload = (await response.json()) as ErrorPayload;
    return payload.error?.message ?? `request failed (${response.status})`;
  } catch {
    return `request failed (${response.status})`;
  }
}

async function requestJson<T>(
    baseUrl: string, path: string, init?: RequestInit): Promise<T> {
  const response = await fetch(`${baseUrl}${path}`, init);
  if (!response.ok) {
    throw new Error(await parseApiError(response));
  }
  return (await response.json()) as T;
}

export async function fetchBananaSummary(baseUrl: string):
    Promise<BananaSummaryResponse> {
  return requestJson<BananaSummaryResponse>(
      baseUrl, '/banana?purchases=3&multiplier=2');
}

export async function createChatSession(
    baseUrl: string,
    platform: string,
    ): Promise<CreateSessionResponse> {
  return requestJson<CreateSessionResponse>(baseUrl, '/chat/sessions', {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify({
      platform,
      metadata: {source: 'react-portal'},
    }),
  });
}

export async function sendChatMessage(
    baseUrl: string,
    sessionId: string,
    content: string,
    clientMessageId: string,
    ): Promise<SendMessageResponse> {
  return requestJson<SendMessageResponse>(
      baseUrl, `/chat/sessions/${sessionId}/messages`, {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({
          content,
          client_message_id: clientMessageId,
        }),
      });
}

export async function predictRipeness(
    baseUrl: string,
    inputJson: string,
    ): Promise<RipenessResponse> {
  return requestJson<RipenessResponse>(baseUrl, '/ripeness/predict', {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify({inputJson}),
  });
}

/**
 * Slice 015 -- typed helper for the slice 014 `POST /ml/ensemble` route.
 * The response shape is locked by the snapshot test in `api.test.ts`; any
 * field-name drift on the managed side will fail this lane.
 */
export async function fetchEnsembleVerdict(
    baseUrl: string,
    text: string,
    ): Promise<EnsembleVerdict> {
  return requestJson<EnsembleVerdict>(baseUrl, '/ml/ensemble', {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify({inputJson: JSON.stringify({text})}),
  });
}

/**
 * Slice 023 -- typed helper for the slice 017
 * `POST /ml/ensemble/embedding` route. Returns the legacy verdict plus
 * the 4-dim embedding fingerprint when the cascade escalated; embedding
 * is null on cheap-path verdicts. Field shape is snapshot-locked in
 * `api.test.ts`.
 */
export type EnsembleVerdictWithEmbedding = {
  verdict: EnsembleVerdict; embedding: number[] | null;
};

export async function fetchEnsembleVerdictWithEmbedding(
    baseUrl: string,
    text: string,
    ): Promise<EnsembleVerdictWithEmbedding> {
  return requestJson<EnsembleVerdictWithEmbedding>(
      baseUrl, '/ml/ensemble/embedding', {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({inputJson: JSON.stringify({text})}),
      });
}

export async function runTrainingWorkbench(
    baseUrl: string,
    payload: TrainingRunRequest,
    ): Promise<TrainingRunResponse> {
  return requestJson<TrainingRunResponse>(baseUrl, '/training/workbench/runs', {
    method: 'POST',
    headers: {'content-type': 'application/json'},
    body: JSON.stringify(payload),
  });
}

export async function fetchTrainingWorkbenchHistory(
    baseUrl: string,
    lane?: TrainingLane,
    ): Promise<TrainingHistoryResponse> {
  const suffix = lane ? `?lane=${encodeURIComponent(lane)}` : '';
  return requestJson<TrainingHistoryResponse>(
      baseUrl, `/training/workbench/history${suffix}`);
}

export async function fetchTrainingWorkbenchRun(
    baseUrl: string,
    runId: string,
    ): Promise<TrainingRunDetailResponse> {
  return requestJson<TrainingRunDetailResponse>(
      baseUrl, `/training/workbench/runs/${encodeURIComponent(runId)}`);
}

export async function promoteTrainingWorkbenchRun(
    baseUrl: string,
    runId: string,
    target: 'candidate'|'stable',
    operator_id: string,
    reason?: string,
    ): Promise<PromoteRunResponse> {
  return requestJson<PromoteRunResponse>(
      baseUrl,
      `/training/workbench/runs/${encodeURIComponent(runId)}/promote`,
      {
        method: 'POST',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({target, operator_id, reason}),
      },
  );
}
