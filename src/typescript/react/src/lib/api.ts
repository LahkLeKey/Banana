import type {ChatMessage, ChatSession, EnsembleVerdict, Ripeness, TrainingAuditEvent, TrainingLane, TrainingRunRequest, TrainingRunResult,} from '@banana/ui';

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
  chatApiBaseUrl?: string;
};

export type ApiBaseResolution = {
  baseUrl: string; error: string | null;
  source: 'vite' | 'electron' | 'localhost-default' | 'unresolved';
};

export type ChatBootstrapErrorKind =
    'transport_unreachable'|'request_failed'|'unexpected';

export type ChatBootstrapErrorResolution = {
  kind: ChatBootstrapErrorKind; message: string; remediation: string;
  retryable: boolean;
};

function resolveElectronBridge(): ElectronBridge|undefined {
  return typeof window !== 'undefined' ? window.banana : undefined;
}

function resolveViteApiBaseUrl(): string {
  const importMeta = import.meta as {env?: {VITE_BANANA_API_BASE_URL?: string}};
  return importMeta.env?.VITE_BANANA_API_BASE_URL ?? '';
}

function resolveViteChatApiBaseUrl(): string {
  const importMeta =
      import.meta as {env?: {VITE_BANANA_CHAT_API_BASE_URL?: string}};
  return importMeta.env?.VITE_BANANA_CHAT_API_BASE_URL ?? '';
}

function resolveLocalhostDefaultApiBaseUrl(): string {
  if (typeof window === 'undefined') {
    return '';
  }

  const host = window.location?.hostname?.trim().toLowerCase() ?? '';
  if (host === 'localhost' || host === '127.0.0.1') {
    return 'http://localhost:8080';
  }

  return '';
}

export function resolveApiBaseResolution(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): ApiBaseResolution {
  const normalizedVite = viteBaseUrl.trim();
  if (normalizedVite.length > 0) {
    return {baseUrl: normalizedVite, error: null, source: 'vite'};
  }

  const normalizedElectron = (electronBridge?.apiBaseUrl ?? '').trim();
  if (normalizedElectron.length > 0) {
    return {baseUrl: normalizedElectron, error: null, source: 'electron'};
  }

  const normalizedLocalhostDefault = localhostDefault.trim();
  if (normalizedLocalhostDefault.length > 0) {
    return {
      baseUrl: normalizedLocalhostDefault,
      error: null,
      source: 'localhost-default',
    };
  }

  return {
    baseUrl: '',
    error:
        'Missing API base URL. Set VITE_BANANA_API_BASE_URL for web or BANANA_API_BASE_URL for Electron preload, then relaunch via the canonical Compose frontend profile.',
    source: 'unresolved',
  };
}

export function resolveApiBaseResolutionError(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): string|null {
  return resolveApiBaseResolution(viteBaseUrl, electronBridge, localhostDefault)
      .error;
}

export function resolveApiBaseUrl(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge(),
    localhostDefault = resolveLocalhostDefaultApiBaseUrl()): string {
  return resolveApiBaseResolution(viteBaseUrl, electronBridge, localhostDefault)
      .baseUrl;
}

export function resolvePlatformLabel(electronBridge = resolveElectronBridge()):
    string {
  return electronBridge?.platform ? `electron-${electronBridge.platform}` :
                                    'web';
}

function resolveLocalhostFastifyChatBase(baseUrl: string): string {
  if (baseUrl.trim().length === 0) {
    return '';
  }

  try {
    const parsed = new URL(baseUrl);
    const host = parsed.hostname.trim().toLowerCase();
    if ((host === 'localhost' || host === '127.0.0.1') &&
        parsed.port === '8080') {
      parsed.port = '8081';
      return parsed.toString().replace(/\/$/, '');
    }
  } catch {
    return '';
  }

  return '';
}

export function resolveChatApiBaseUrl(
    apiBaseUrl: string, viteChatApiBaseUrl = resolveViteChatApiBaseUrl(),
    electronBridge = resolveElectronBridge()): string {
  const normalizedVite = viteChatApiBaseUrl.trim();
  if (normalizedVite.length > 0) {
    return normalizedVite;
  }

  const normalizedElectron = (electronBridge?.chatApiBaseUrl ?? '').trim();
  if (normalizedElectron.length > 0) {
    return normalizedElectron;
  }

  const localhostFastify = resolveLocalhostFastifyChatBase(apiBaseUrl);
  if (localhostFastify.length > 0) {
    return localhostFastify;
  }

  return apiBaseUrl;
}

export function resolveChatBootstrapError(error: unknown):
    ChatBootstrapErrorResolution {
  if (error instanceof Error) {
    const message = error.message.trim();
    const normalized = message.toLowerCase();

    if (normalized.includes('failed to fetch') ||
        normalized.includes('network')) {
      return {
        kind: 'transport_unreachable',
        message:
            'Chat bootstrap transport failure: the chat runtime endpoint could not be reached.',
        remediation:
            'Ensure the runtime profile is healthy and the apps profile is up, then retry chat bootstrap or reload.',
        retryable: true,
      };
    }

    if (normalized.startsWith('request failed')) {
      return {
        kind: 'request_failed',
        message: `Chat bootstrap request failed: ${message}`,
        remediation:
            'Verify chat session routes are available for the configured API base, then retry.',
        retryable: true,
      };
    }

    return {
      kind: 'unexpected',
      message: `Chat bootstrap failed: ${message}`,
      remediation:
          'Check runtime logs and frontend diagnostics, then retry after remediation.',
      retryable: true,
    };
  }

  return {
    kind: 'unexpected',
    message: 'Chat bootstrap failed: unknown error',
    remediation:
        'Check runtime logs and frontend diagnostics, then retry after remediation.',
    retryable: true,
  };
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
    chatApiBaseUrl = resolveChatApiBaseUrl(baseUrl),
    ): Promise<CreateSessionResponse> {
  return requestJson<CreateSessionResponse>(chatApiBaseUrl, '/chat/sessions', {
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
    chatApiBaseUrl = resolveChatApiBaseUrl(baseUrl),
    ): Promise<SendMessageResponse> {
  return requestJson<SendMessageResponse>(
      chatApiBaseUrl, `/chat/sessions/${sessionId}/messages`, {
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
