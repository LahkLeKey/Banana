import type {NativeNetcodeService} from './nativeNetcode.ts';
import type {NetcodeAnalyticsAuthoritativeRequest, NetcodeAnalyticsAuthoritativeResult, NetcodeHypersphereRollout,} from './netcodeAuthoritativeComputeOrchestrator.ts';
import {createK3h4ApplicationService, type K3h4ApplicationService,} from '../application/k3h4/k3h4ApplicationService.ts';
import {createNativeK3h4AuthoritativeAnalyticsAdapter,} from '../infrastructure/k3h4/nativeK3h4AuthoritativeAnalyticsAdapter.ts';

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
  const analyticsPort = createNativeK3h4AuthoritativeAnalyticsAdapter(netcode);
  const applicationService = createK3h4ApplicationService(analyticsPort);
  return new DefaultK3h4ApplicationOrchestrationLayer(applicationService);
}
