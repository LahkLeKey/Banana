import type {BananaMlBinaryRequest, BananaMlBinaryResponse, BananaMlRegressionRequest, BananaMlRegressionResponse, BananaMlTransformerRequest, BananaMlTransformerResponse,} from '../../contracts/http.js';
import {type ProxyResponse, requestJson} from '../../shared/httpClient.js';

export async function proxyMlRegression(
    legacyMlBaseUrl: string,
    request: BananaMlRegressionRequest,
    ): Promise<ProxyResponse<BananaMlRegressionResponse>> {
  const endpoint = new URL('/ml/regression', legacyMlBaseUrl);
  return requestJson<BananaMlRegressionResponse>(endpoint, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(request),
  });
}

export async function proxyMlBinary(
    legacyMlBaseUrl: string,
    request: BananaMlBinaryRequest,
    ): Promise<ProxyResponse<BananaMlBinaryResponse>> {
  const endpoint = new URL('/ml/binary', legacyMlBaseUrl);
  return requestJson<BananaMlBinaryResponse>(endpoint, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(request),
  });
}

export async function proxyMlTransformer(
    legacyMlBaseUrl: string,
    request: BananaMlTransformerRequest,
    ): Promise<ProxyResponse<BananaMlTransformerResponse>> {
  const endpoint = new URL('/ml/transformer', legacyMlBaseUrl);
  return requestJson<BananaMlTransformerResponse>(endpoint, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(request),
  });
}
