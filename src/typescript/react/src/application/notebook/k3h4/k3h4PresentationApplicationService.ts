import type {K3h4PresentationMapperPort} from '../../../domain/notebook/k3h4/k3h4PresentationDomain';
import type {K3h4AnalyticsPresentationState} from '../../../domain/notebook/k3h4AnalyticsOrchestrationLayer';
import type {NetcodeAnalyticsResponse} from '../../../lib/api';

/**
 * Application-facing mapper for notebook k3h4 presentation state.
 * It keeps notebook UI code dependent on one narrow mapping interface rather
 * than on the lower-level domain transformation directly.
 */
export interface K3h4PresentationApplicationService {
  map(analytics: NetcodeAnalyticsResponse): K3h4AnalyticsPresentationState;
}

class DefaultK3h4PresentationApplicationService implements
    K3h4PresentationApplicationService {
  constructor(private readonly mapperPort: K3h4PresentationMapperPort) {}

  public map(
      analytics: NetcodeAnalyticsResponse,
      ): K3h4AnalyticsPresentationState {
    return this.mapperPort.map(analytics);
  }
}

export function createK3h4PresentationApplicationService(
    mapperPort: K3h4PresentationMapperPort,
    ): K3h4PresentationApplicationService {
  return new DefaultK3h4PresentationApplicationService(mapperPort);
}
