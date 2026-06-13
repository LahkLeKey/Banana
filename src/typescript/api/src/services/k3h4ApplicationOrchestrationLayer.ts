import type {NativeNetcodeService} from './nativeNetcode.ts';
import {createNetcodeAnalyticsAuthoritativeComputeOrchestrator, type NetcodeAnalyticsAuthoritativeComputeOrchestrator, type NetcodeAnalyticsAuthoritativeRequest, type NetcodeAnalyticsAuthoritativeResult, type NetcodeHypersphereRollout,} from './netcodeAuthoritativeComputeOrchestrator.ts';

export interface K3h4ApplicationOrchestrationLayer {
  compute(
      request: NetcodeAnalyticsAuthoritativeRequest,
      rollout: NetcodeHypersphereRollout,
      ): Promise<NetcodeAnalyticsAuthoritativeResult>;
}

class DefaultK3h4ApplicationOrchestrationLayer implements
    K3h4ApplicationOrchestrationLayer {
  constructor(
      private readonly delegate:
          NetcodeAnalyticsAuthoritativeComputeOrchestrator,
  ) {}

  public compute(
      request: NetcodeAnalyticsAuthoritativeRequest,
      rollout: NetcodeHypersphereRollout,
      ): Promise<NetcodeAnalyticsAuthoritativeResult> {
    return this.delegate.compute(request, rollout);
  }
}

export function createK3h4ApplicationOrchestrationLayer(
    netcode: NativeNetcodeService,
    ): K3h4ApplicationOrchestrationLayer {
  return new DefaultK3h4ApplicationOrchestrationLayer(
      createNetcodeAnalyticsAuthoritativeComputeOrchestrator(netcode));
}
