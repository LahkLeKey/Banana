export type WorkflowStepId = 'scan'|'shape'|'commit';
export type WorkflowDepth = 1|2|3;

export type WorkflowStepViewModel = {
  readonly label: string; readonly summary: string; readonly detailA: string; readonly detailB:
                                                                                           string;
};

export type WorkflowStepMap = Record<WorkflowStepId, WorkflowStepViewModel>;

export type WorkflowModelInput =
    {
      readonly missionTitle: string; readonly hotspotScore: number; readonly riskLabel: string; readonly callDensity: number; readonly primaryDependencyLabel: string; readonly branchPressure: number; readonly dependencyPulse: number; readonly nextDomainTarget: string; readonly completedQuestCount: number; readonly totalQuestCount: number; readonly comboStreak: number; readonly lastXpGain:
                                                                                                                                                                                                                                                                                                                                                                                                number;
    };

export function buildWorkflowStepMap(input: WorkflowModelInput):
    WorkflowStepMap {
  return {
    scan: {
      label: 'Signal Scan',
      summary: `Sweep ${input.missionTitle} for pressure and route confidence.`,
      detailA: `Hotspot ${input.hotspotScore} · ${input.riskLabel} risk`,
      detailB: `Readiness ${Math.min(100, 42 + input.callDensity)}%`,
    },
    shape: {
      label: 'Route Shape',
      summary: `Shape mission lanes around ${input.primaryDependencyLabel}.`,
      detailA: `Frontline ${input.branchPressure}% · Logistics ${
          input.dependencyPulse}%`,
      detailB: `Target lane ${input.nextDomainTarget.toUpperCase()}`,
    },
    commit: {
      label: 'Deploy Commit',
      summary: 'Commit the current step and arm dock actions.',
      detailA: `${input.completedQuestCount}/${
          input.totalQuestCount} objectives complete`,
      detailB:
          `Combo x${Math.max(1, input.comboStreak)} · XP +${input.lastXpGain}`,
    },
  };
}
