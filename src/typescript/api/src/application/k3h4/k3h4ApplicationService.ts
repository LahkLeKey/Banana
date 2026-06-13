import type {K3h4AuthoritativeAnalyticsPort, K3h4AuthoritativeRequest, K3h4AuthoritativeResult, K3h4Rollout,} from '../../domain/k3h4/k3h4AuthoritativeAnalytics.ts';

export interface K3h4ApplicationService {
  compute(
      request: K3h4AuthoritativeRequest,
      rollout: K3h4Rollout,
      ): Promise<K3h4AuthoritativeResult>;
}

class DefaultK3h4ApplicationService implements K3h4ApplicationService {
  constructor(private readonly analyticsPort: K3h4AuthoritativeAnalyticsPort) {}

  public compute(
      request: K3h4AuthoritativeRequest,
      rollout: K3h4Rollout,
      ): Promise<K3h4AuthoritativeResult> {
    return this.analyticsPort.compute(request, rollout);
  }
}

export function createK3h4ApplicationService(
    analyticsPort: K3h4AuthoritativeAnalyticsPort,
    ): K3h4ApplicationService {
  return new DefaultK3h4ApplicationService(analyticsPort);
}
