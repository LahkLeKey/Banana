export const SHARED_ORCHESTRATION_PATH_ID = 'baseline-plus-delta-replay-v1';

export type OrchestrationPhase = 'initial-generation'|'revisit-reconstruction';

export interface OrchestrationPathSelection {
  readonly phase: OrchestrationPhase;
  readonly pathId: string;
}

export interface OrchestrationPathService {
  selectPath(phase: OrchestrationPhase): OrchestrationPathSelection;
  assertSharedPath(
      initialPathId: string,
      revisitPathId: string,
      ): void;
}

export function createOrchestrationPathService(): OrchestrationPathService {
  return {
    selectPath(phase) {
      return {
        phase,
        pathId: SHARED_ORCHESTRATION_PATH_ID,
      };
    },
    assertSharedPath(initialPathId, revisitPathId) {
      if (initialPathId !== revisitPathId) {
        throw new Error(
            `Orchestration path mismatch: expected same path for initial/revisit, got initial=${
                initialPathId}, revisit=${revisitPathId}`);
      }
    },
  };
}
