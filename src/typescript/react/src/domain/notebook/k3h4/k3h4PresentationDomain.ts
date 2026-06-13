import type {NetcodeAnalyticsResponse} from '../../../lib/api';
import type {K3h4AnalyticsPresentationState} from '../k3h4AnalyticsOrchestrationLayer';

export interface K3h4PresentationMapperPort {
  map(analytics: NetcodeAnalyticsResponse): K3h4AnalyticsPresentationState;
}
