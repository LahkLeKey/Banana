import {useCallback, useEffect, useState} from 'react';

import {fetchK3h4ScalingBenchmark, type K3h4ScalingBenchmarkResponse, resolveApiBaseUrl,} from '../../../lib/api';

/** UI state machine for the optional native k3h4 scaling benchmark artifact. */
export type K3h4ScalingBenchmarkState =|{status: 'idle'}|{status: 'loading'}|{
  status: 'ok';
  data: K3h4ScalingBenchmarkResponse
}
|{status: 'not_found'}|{
  status: 'error';
  message: string
};

/**
 * Loads the benchmark artifact through the API and normalizes absent artifacts
 * into a dedicated not_found state for the notebook visualization.
 */
export function useK3h4ScalingBenchmark():
    {state: K3h4ScalingBenchmarkState; refresh: () => void;} {
  const [state, setState] =
      useState<K3h4ScalingBenchmarkState>({status: 'idle'});

  const load = useCallback(async () => {
    setState({status: 'loading'});
    const baseUrl = resolveApiBaseUrl();
    try {
      const data = await fetchK3h4ScalingBenchmark(baseUrl);
      setState({status: 'ok', data});
    } catch (err) {
      const message = err instanceof Error ? err.message : String(err);
      if (message.includes('benchmark_not_found') || message.includes('404')) {
        setState({status: 'not_found'});
      } else {
        setState({status: 'error', message});
      }
    }
  }, []);

  useEffect(() => {
    void load();
  }, [load]);

  return {state, refresh: load};
}
