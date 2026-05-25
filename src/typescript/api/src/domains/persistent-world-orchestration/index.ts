import type {FastifyInstance} from 'fastify';

import {type AreaStateVersionService, createAreaStateVersionService} from './services/areaStateVersionService.ts';
import {createFailureContextService, type FailureContextService} from './services/failureContextService.ts';
import {createOrchestrationPathService, type OrchestrationPathService} from './services/orchestrationPathService.ts';
import {createReplayMergeService, type ReplayMergeService} from './services/replayMergeService.ts';
import {createRetryOrchestrationService, type RetryOrchestrationService} from './services/retryOrchestrationService.ts';
import {createRevisitBaselineService, type RevisitBaselineService} from './services/revisitBaselineService.ts';
import {createStorageBudgetService, type StorageBudgetService} from './services/storageBudgetService.ts';
import {createVersionDriftService, type VersionDriftService} from './services/versionDriftService.ts';

export const PERSISTENT_WORLD_ORCHESTRATION_DOMAIN =
    'persistent-world-orchestration';

export interface PersistentWorldOrchestrationDomain {
  readonly contractVersion: string;
  readonly orchestrationPathService: OrchestrationPathService;
  readonly revisitBaselineService: RevisitBaselineService;
  readonly areaStateVersionService: AreaStateVersionService;
  readonly replayMergeService: ReplayMergeService;
  readonly retryOrchestrationService: RetryOrchestrationService;
  readonly failureContextService: FailureContextService;
  readonly storageBudgetService: StorageBudgetService;
  readonly versionDriftService: VersionDriftService;
}

export function bootstrapPersistentWorldOrchestrationDomain(
    app: FastifyInstance): PersistentWorldOrchestrationDomain {
  const orchestrationPathService = createOrchestrationPathService();
  const areaStateVersionService = createAreaStateVersionService();
  const replayMergeService = createReplayMergeService(
      undefined,
      undefined,
      areaStateVersionService,
  );
  const retryOrchestrationService = createRetryOrchestrationService();
  const failureContextService = createFailureContextService();
  const storageBudgetService = createStorageBudgetService();
  const versionDriftService = createVersionDriftService();
  const revisitBaselineService =
      createRevisitBaselineService(orchestrationPathService);

  const domain: PersistentWorldOrchestrationDomain = {
    contractVersion: 'v1',
    orchestrationPathService,
    revisitBaselineService,
    areaStateVersionService,
    replayMergeService,
    retryOrchestrationService,
    failureContextService,
    storageBudgetService,
    versionDriftService,
  };

  app.log.info(
      {
        domain: PERSISTENT_WORLD_ORCHESTRATION_DOMAIN,
        contractVersion: domain.contractVersion,
      },
      'persistent-world orchestration domain bootstrapped');

  return domain;
}
