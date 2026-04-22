import type {BananaRequest, BananaResponse} from '../../contracts/http.js';
import {type ProxyResponse, requestJson} from '../../shared/httpClient.js';

export async function proxyBananaCalculation(
    legacyApiBaseUrl: string,
    request: BananaRequest,
    ): Promise<ProxyResponse<BananaResponse>> {
  const endpoint = new URL('/banana', legacyApiBaseUrl);
  endpoint.searchParams.set('purchases', String(request.purchases));
  endpoint.searchParams.set('multiplier', String(request.multiplier));

  return requestJson<BananaResponse>(endpoint, {
    method: 'GET',
    headers: {Accept: 'application/json'},
  });
}
