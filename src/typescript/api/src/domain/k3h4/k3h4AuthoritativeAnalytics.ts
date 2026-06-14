import type {NetcodeAnalyticsAuthoritativeRequest, NetcodeAnalyticsAuthoritativeResult, NetcodeK3h4Rollout,} from '../../services/netcodeAuthoritativeComputeOrchestrator.ts';

export type K3h4AuthoritativeRequest = NetcodeAnalyticsAuthoritativeRequest;
export type K3h4AuthoritativeResult = NetcodeAnalyticsAuthoritativeResult;
export type K3h4Rollout = NetcodeK3h4Rollout;

export interface K3h4AuthoritativeAnalyticsPort {
  compute(
      request: K3h4AuthoritativeRequest,
      rollout: K3h4Rollout,
      ): Promise<K3h4AuthoritativeResult>;
}
