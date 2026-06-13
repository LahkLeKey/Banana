import {composeK3h4ApplicationService,} from '../composition/k3h4/k3h4CompositionRoot.ts';
import type {K3h4ApplicationService} from '../application/k3h4/k3h4ApplicationService.ts';

import type {NativeNetcodeService} from './nativeNetcode.ts';
import type {NetcodeAnalyticsAuthoritativeRequest, NetcodeAnalyticsAuthoritativeResult, NetcodeHypersphereRollout,} from './netcodeAuthoritativeComputeOrchestrator.ts';

export interface K3h4ApplicationOrchestrationLayer {
  compute(
      request: NetcodeAnalyticsAuthoritativeRequest,
      rollout: NetcodeHypersphereRollout,
      ): Promise<NetcodeAnalyticsAuthoritativeResult>;
}

class DefaultK3h4ApplicationOrchestrationLayer implements
    K3h4ApplicationOrchestrationLayer {
  constructor(
      private readonly delegate: K3h4ApplicationService,
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
  const applicationService = composeK3h4ApplicationService(netcode);
  return new DefaultK3h4ApplicationOrchestrationLayer(applicationService);
}
