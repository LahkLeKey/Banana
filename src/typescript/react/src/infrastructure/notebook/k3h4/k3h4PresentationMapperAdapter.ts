import type {K3h4PresentationMapperPort} from '../../../domain/notebook/k3h4/k3h4PresentationDomain';
import {mapK3h4AnalyticsToPresentationState,} from '../../../domain/notebook/k3h4AnalyticsOrchestrationLayer';

export function createK3h4PresentationMapperAdapter():
    K3h4PresentationMapperPort {
  return {
    map: (analytics) => mapK3h4AnalyticsToPresentationState(analytics),
  };
}
