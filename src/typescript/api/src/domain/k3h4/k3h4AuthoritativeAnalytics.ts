import type {NetcodeAnalyticsAuthoritativeRequest, NetcodeAnalyticsAuthoritativeResult, NetcodeHypersphereRollout,} from '../../services/netcodeAuthoritativeComputeOrchestrator.ts';

export type K3h4AuthoritativeRequest = NetcodeAnalyticsAuthoritativeRequest;
export type K3h4AuthoritativeResult = NetcodeAnalyticsAuthoritativeResult;
export type K3h4Rollout = NetcodeHypersphereRollout;

export interface K3h4AuthoritativeAnalyticsPort {
  compute(
      request: K3h4AuthoritativeRequest,
      rollout: K3h4Rollout,
      ): Promise<K3h4AuthoritativeResult>;
}
