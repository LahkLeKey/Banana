import type {NetcodeAnalyticsResponse} from '../../../lib/api';
import type {K3h4AnalyticsPresentationState} from '../k3h4AnalyticsOrchestrationLayer';

/** Domain port that maps API analytics responses into notebook presentation state. */
export interface K3h4PresentationMapperPort {
  map(analytics: NetcodeAnalyticsResponse): K3h4AnalyticsPresentationState;
}
