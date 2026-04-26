import type {ChatMessage, ChatSession, Ripeness} from '@banana/ui';

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

export function resolveApiBaseUrl(
    viteBaseUrl = resolveViteApiBaseUrl(),
    electronBridge = resolveElectronBridge()): string {
  return viteBaseUrl || electronBridge?.apiBaseUrl || '';
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
