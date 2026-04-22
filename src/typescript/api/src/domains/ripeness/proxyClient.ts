import type {BananaRipenessRequest, BananaRipenessResponse} from '../../contracts/http.js';
import {type ProxyResponse, requestJson} from '../../shared/httpClient.js';

export async function proxyRipenessPrediction(
    legacyApiBaseUrl: string,
    request: BananaRipenessRequest,
    ): Promise<ProxyResponse<BananaRipenessResponse>> {
  const endpoint = new URL('/ripeness/predict', legacyApiBaseUrl);

  return requestJson<BananaRipenessResponse>(endpoint, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(request),
  });
}
