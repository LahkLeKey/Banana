import type {BananaBatchCreateRequest, BananaBatchResponse, BananaRequest, BananaResponse, BananaRipenessRequest, BananaRipenessResponse, HealthResponse,} from '../types/api';

const DEFAULT_BASE_URL = 'http://localhost:5000';

function getBaseUrl(): string {
  const configured = import.meta.env.VITE_BANANA_API_BASE_URL;
  if (configured && configured.trim().length > 0) {
    return configured;
  }

  return DEFAULT_BASE_URL;
}

async function parseJson<T>(response: Response): Promise<T> {
  const text = await response.text();
  return (text.length > 0 ? JSON.parse(text) : null) as T;
}

async function unwrap<T>(response: Response): Promise<T> {
  if (!response.ok) {
    const payload = await parseJson<unknown>(response).catch(() => null);
    throw new Error(`API ${response.status} ${response.statusText}: ${
        JSON.stringify(payload)}`);
  }

  return parseJson<T>(response);
}

export async function fetchHealth(signal?: AbortSignal):
    Promise<HealthResponse> {
  const endpoint = new URL('/health', getBaseUrl());
  const response = await fetch(
      endpoint, {method: 'GET', headers: {Accept: 'application/json'}, signal});

  return unwrap<HealthResponse>(response);
}

export async function fetchBanana(
    request: BananaRequest, signal?: AbortSignal): Promise<BananaResponse> {
  const endpoint = new URL('/banana', getBaseUrl());
  endpoint.searchParams.set('purchases', String(request.purchases));
  endpoint.searchParams.set('multiplier', String(request.multiplier));

  const response = await fetch(
      endpoint, {method: 'GET', headers: {Accept: 'application/json'}, signal});

  return unwrap<BananaResponse>(response);
}

export async function createBatch(
    request: BananaBatchCreateRequest,
    signal?: AbortSignal): Promise<BananaBatchResponse> {
  const endpoint = new URL('/batches/create', getBaseUrl());
  const response = await fetch(endpoint, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(request),
    signal,
  });

  return unwrap<BananaBatchResponse>(response);
}

export async function fetchBatchStatus(
    batchId: string, signal?: AbortSignal): Promise<BananaBatchResponse> {
  const endpoint =
      new URL(`/batches/${encodeURIComponent(batchId)}/status`, getBaseUrl());
  const response = await fetch(endpoint, {
    method: 'GET',
    headers: {Accept: 'application/json'},
    signal,
  });

  return unwrap<BananaBatchResponse>(response);
}

export async function predictRipeness(
    request: BananaRipenessRequest,
    signal?: AbortSignal): Promise<BananaRipenessResponse> {
  const endpoint = new URL('/ripeness/predict', getBaseUrl());
  const response = await fetch(endpoint, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(request),
    signal,
  });

  return unwrap<BananaRipenessResponse>(response);
}
