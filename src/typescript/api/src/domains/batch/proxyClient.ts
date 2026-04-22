import type {BananaBatchCreateRequest, BananaBatchResponse} from '../../contracts/http.js';
import {type ProxyResponse, requestJson} from '../../shared/httpClient.js';

export async function proxyBatchCreate(
    legacyApiBaseUrl: string,
    request: BananaBatchCreateRequest,
    ): Promise<ProxyResponse<BananaBatchResponse>> {
  const endpoint = new URL('/batches/create', legacyApiBaseUrl);

  return requestJson<BananaBatchResponse>(endpoint, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(request),
  });
}

export async function proxyBatchStatus(
    legacyApiBaseUrl: string,
    batchId: string,
    ): Promise<ProxyResponse<BananaBatchResponse>> {
  const endpoint = new URL(
      `/batches/${encodeURIComponent(batchId)}/status`, legacyApiBaseUrl);

  return requestJson<BananaBatchResponse>(endpoint, {
    method: 'GET',
    headers: {Accept: 'application/json'},
  });
}
