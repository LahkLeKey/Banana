// React Native API client (slice 025).
// Mirrors src/typescript/react/src/lib/api.ts. Both modules are typed
// against the @banana/ui contract types so that any drift on the
// managed side fails tsc here too.

import type { EnsembleVerdict } from "@banana/ui/native";

type ErrorPayload = {
  error?: { message?: string };
};

export type EnsembleVerdictWithEmbedding = {
  verdict: EnsembleVerdict;
  embedding: number[] | null;
};

async function parseApiError(response: Response): Promise<string> {
  try {
    const payload = (await response.json()) as ErrorPayload;
    return payload.error?.message ?? `request failed (${response.status})`;
  } catch {
    return `request failed (${response.status})`;
  }
}

async function requestJson<T>(baseUrl: string, path: string, init?: RequestInit): Promise<T> {
  const response = await fetch(`${baseUrl}${path}`, init);
  if (!response.ok) {
    throw new Error(await parseApiError(response));
  }
  return (await response.json()) as T;
}

export async function fetchEnsembleVerdict(
  baseUrl: string,
  text: string
): Promise<EnsembleVerdict> {
  return requestJson<EnsembleVerdict>(baseUrl, "/ml/ensemble", {
    method: "POST",
    headers: { "content-type": "application/json" },
    body: JSON.stringify({ inputJson: JSON.stringify({ text }) }),
  });
}

export async function fetchEnsembleVerdictWithEmbedding(
  baseUrl: string,
  text: string
): Promise<EnsembleVerdictWithEmbedding> {
  return requestJson<EnsembleVerdictWithEmbedding>(baseUrl, "/ml/ensemble/embedding", {
    method: "POST",
    headers: { "content-type": "application/json" },
    body: JSON.stringify({ inputJson: JSON.stringify({ text }) }),
  });
}
