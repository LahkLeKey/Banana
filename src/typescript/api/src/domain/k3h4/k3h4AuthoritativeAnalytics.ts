import type {NetcodeAnalyticsAuthoritativeRequest, NetcodeAnalyticsAuthoritativeResult, NetcodeK3h4Rollout,} from '../../services/netcodeAuthoritativeComputeOrchestrator.ts';

export type K3h4AuthoritativeRequest = NetcodeAnalyticsAuthoritativeRequest;
export type K3h4AuthoritativeResult = NetcodeAnalyticsAuthoritativeResult;
export type K3h4Rollout = NetcodeK3h4Rollout;

/**
 * Domain port for the authoritative k3h4 compute flow.
 * The domain keeps the API/native orchestration behind this port so the
 * application layer depends on an intent, not on FFI-specific details.
 */
export interface K3h4AuthoritativeAnalyticsPort {
  compute(
      request: K3h4AuthoritativeRequest,
      rollout: K3h4Rollout,
      ): Promise<K3h4AuthoritativeResult>;
}
