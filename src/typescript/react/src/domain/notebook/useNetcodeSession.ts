import {useCallback, useEffect, useRef, useState} from 'react';

import {fetchNetcodeAnalytics, fetchNetcodeLearning, NetcodeAnalyticsError, resolveApiBaseUrl,} from '../../lib/api';

import type {ContractHypersphereKmeansModel, ContractHypersphereProjectionModel, ContractNodeId, ContractNodeVectorModel, NetcodeAnalyticsAvailabilityModel, NodeInteractionAction, NodeInteractionLearningModel, NodeInteractionLedger, NodeLinkConfidenceModel, RewardSignalModel,} from './network-domain';
import {mapK3h4AnalyticsToPresentationState,} from './k3h4AnalyticsOrchestrationLayer';

type NetcodeSignals = {
  readonly callDensity: number; readonly questPercent: number; readonly comboStreak: number; readonly branchPressure: number; readonly workflowDepth: 1 | 2 | 3; readonly playerLevel: number; readonly dependencyPulse: number; readonly networkDimensions:
                                                                                                                                                                                                                                              number;
};

const LABELING_BAND = {
  line: '#f97316',
  soft: 'rgba(249, 115, 22, 0.24)',
  glow: 'rgba(249, 115, 22, 0.36)',
} as const;

const DEFAULT_LEARNING_MODEL: NodeInteractionLearningModel = {
  modelConfidence: 0,
  trainingAccuracy: 0,
  policyMomentum: 0,
  nodeWeights: {intel: 0, objectives: 0, player: 0, ops: 0},
  recommendedNode: 'intel',
  recommendedAction: 'snapshot',
  xpByAction: {snapshot: 4, inspect: 5, train: 7, route: 8},
};

const DEFAULT_LEDGER: NodeInteractionLedger = {
  snapshots: 0,
  inspections: 0,
  trainings: 0,
  routes: 0,
  nodeTaps: 0,
};

const DEFAULT_REWARD_SIGNAL: RewardSignalModel = {
  neuralRelevanceScore: 0,
  projectedRewardXp: 5,
  rewardTier: 'Needs Labeling',
  rewardBand: LABELING_BAND,
};

const DEFAULT_LINK_CONFIDENCE: NodeLinkConfidenceModel = {
  intel: 0,
  objectives: 0,
  player: 0,
  ops: 0,
};

function makeDefaultProjectionNode(id: ContractNodeId) {
  return {
    id,
    x: 0,
    y: 0,
    z: 0,
    coherence: 0,
    inradius: 0,
    nearestNeighborDistance: 0,
  };
}

const DEFAULT_HYPERSPHERE: ContractHypersphereProjectionModel = {
  dimensions: 6,
  nodes: [
    makeDefaultProjectionNode('intel'),
    makeDefaultProjectionNode('objectives'),
    makeDefaultProjectionNode('player'),
    makeDefaultProjectionNode('ops'),
  ],
  alignment: 0,
  radialStability: 0,
};

const DEFAULT_K3H4: ContractHypersphereKmeansModel = {
  centers: [],
  radii: [],
  weightedVoronoiScores: [],
  spectralProxy: [],
  observability: {
    convergenceStatus: 'failed',
    iterationCount: 0,
    assignmentChangesLastIteration: 0,
    scoringValidity: 'invalid',
    deterministicHash: '',
  },
};

const DEFAULT_ANALYTICS_AVAILABILITY: NetcodeAnalyticsAvailabilityModel = {
  available: true,
  reason: 'ok',
  rollout: {
    enabled: true,
    cohort: 'default',
  },
};

function mapNativeNodeId(value: number): ContractNodeId {
  if (value === 1) return 'objectives';
  if (value === 2) return 'player';
  if (value === 3) return 'ops';
  return 'intel';
}

function mapNativeActionId(value: number): NodeInteractionAction {
  if (value === 1) return 'inspect';
  if (value === 2) return 'train';
  if (value === 3) return 'route';
  return 'snapshot';
}

function nodeToIndex(node: ContractNodeId): number {
  if (node === 'objectives') return 1;
  if (node === 'player') return 2;
  if (node === 'ops') return 3;
  return 0;
}

function actionToIndex(action: NodeInteractionAction): number {
  if (action === 'inspect') return 1;
  if (action === 'train') return 2;
  if (action === 'route') return 3;
  return 0;
}

type ApiLearningResponse = {
  ledger: NodeInteractionLedger; learning: {
    modelConfidence: number; trainingAccuracy: number; policyMomentum: number;
    nodeWeights: readonly[number, number, number, number];
    recommendedNode: number;
    recommendedAction: number;
    xpByAction: readonly[number, number, number, number];
  };
};

export type NetcodeSessionHook = {
  readonly learningModel: NodeInteractionLearningModel; readonly ledger: NodeInteractionLedger; readonly recordNodeTap: (node: ContractNodeId) => void; readonly recordAction: (action: NodeInteractionAction) => void; readonly rewardSignal: RewardSignalModel; readonly linkConfidence: NodeLinkConfidenceModel; readonly contractVectors: readonly ContractNodeVectorModel[]; readonly hypersphereProjection: ContractHypersphereProjectionModel; readonly k3h4: ContractHypersphereKmeansModel; readonly analyticsAvailability:
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               NetcodeAnalyticsAvailabilityModel;
};

export function useNetcodeSession({
  callDensity,
  questPercent,
  comboStreak,
  branchPressure,
  workflowDepth,
  playerLevel,
  dependencyPulse,
  networkDimensions,
}: NetcodeSignals): NetcodeSessionHook {
  const [learningModel, setLearningModel] =
      useState<NodeInteractionLearningModel>(DEFAULT_LEARNING_MODEL);
  const [ledger, setLedger] = useState<NodeInteractionLedger>(DEFAULT_LEDGER);
  const [rewardSignal, setRewardSignal] =
      useState<RewardSignalModel>(DEFAULT_REWARD_SIGNAL);
  const [linkConfidence, setLinkConfidence] =
      useState<NodeLinkConfidenceModel>(DEFAULT_LINK_CONFIDENCE);
  const [contractVectors, setContractVectors] =
      useState<readonly ContractNodeVectorModel[]>([]);
  const [hypersphereProjection, setHypersphereProjection] =
      useState<ContractHypersphereProjectionModel>(DEFAULT_HYPERSPHERE);
  const [k3h4, setHypersphereKmeans] =
      useState<ContractHypersphereKmeansModel>(DEFAULT_K3H4);
  const [analyticsAvailability, setAnalyticsAvailability] =
      useState<NetcodeAnalyticsAvailabilityModel>(
          DEFAULT_ANALYTICS_AVAILABILITY);
  const analyticsRequestRef = useRef(0);

  const signalsRef = useRef<NetcodeSignals>({
    callDensity,
    questPercent,
    comboStreak,
    branchPressure,
    workflowDepth,
    playerLevel,
    dependencyPulse,
    networkDimensions,
  });
  signalsRef.current = {
    callDensity,
    questPercent,
    comboStreak,
    branchPressure,
    workflowDepth,
    playerLevel,
    dependencyPulse,
    networkDimensions,
  };

  const applyResponse = useCallback((data: ApiLearningResponse) => {
    setLedger(data.ledger);
    setLearningModel({
      modelConfidence: data.learning.modelConfidence,
      trainingAccuracy: data.learning.trainingAccuracy,
      policyMomentum: data.learning.policyMomentum,
      nodeWeights: {
        intel: data.learning.nodeWeights[0] ?? 0,
        objectives: data.learning.nodeWeights[1] ?? 0,
        player: data.learning.nodeWeights[2] ?? 0,
        ops: data.learning.nodeWeights[3] ?? 0,
      },
      recommendedNode: mapNativeNodeId(data.learning.recommendedNode),
      recommendedAction: mapNativeActionId(data.learning.recommendedAction),
      xpByAction: {
        snapshot: data.learning.xpByAction[0] ?? 4,
        inspect: data.learning.xpByAction[1] ?? 5,
        train: data.learning.xpByAction[2] ?? 7,
        route: data.learning.xpByAction[3] ?? 8,
      },
    });
  }, []);

  const postLearning =
      useCallback(async (extra?: {nodeTap?: number; action?: number}) => {
        const baseUrl = resolveApiBaseUrl();
        if (!baseUrl) return;

        const body = {
          ...signalsRef.current,
          ...extra,
        };

        try {
          const data = await fetchNetcodeLearning(baseUrl, body);
          applyResponse(data as ApiLearningResponse);
        } catch {
          // API unreachable — retain current model values
        }
      }, [applyResponse]);

  useEffect(
      () => {
        void postLearning();
      },
      [
        postLearning, callDensity, questPercent, comboStreak, branchPressure,
        workflowDepth
      ]);

  const recordNodeTap = useCallback((node: ContractNodeId) => {
    void postLearning({nodeTap: nodeToIndex(node)});
  }, [postLearning]);

  const recordAction = useCallback((action: NodeInteractionAction) => {
    void postLearning({action: actionToIndex(action)});
  }, [postLearning]);

  useEffect(
      () => {
        const requestId = analyticsRequestRef.current + 1;
        analyticsRequestRef.current = requestId;

        const baseUrl = resolveApiBaseUrl();
        if (!baseUrl) {
          return;
        }

        const run = async () => {
          try {
            const analytics = await fetchNetcodeAnalytics(baseUrl, {
              callDensity,
              questPercent,
              playerLevel,
              comboStreak,
              branchPressure,
              dependencyPulse,
              workflowDepth,
              networkDimensions,
              modelConfidence: learningModel.modelConfidence,
              policyMomentum: learningModel.policyMomentum,
              interactionSignal: learningModel.modelConfidence,
            });

            if (analyticsRequestRef.current !== requestId) {
              return;
            }

            const presentationState =
                mapK3h4AnalyticsToPresentationState(analytics);
            setRewardSignal(presentationState.rewardSignal);
            setLinkConfidence(presentationState.linkConfidence);
            setContractVectors(presentationState.contractVectors);
            setHypersphereProjection(presentationState.hypersphereProjection);
            setHypersphereKmeans(presentationState.k3h4);

            const rollout = analytics.rollout;
            setAnalyticsAvailability({
              available: rollout.enabled,
              reason: rollout.enabled ? 'ok' : 'rollout-disabled',
              rollout: {
                enabled: rollout.enabled,
                cohort: rollout.cohort,
              },
              contractVersion: analytics.contractVersion,
            });
          } catch (error) {
            if (analyticsRequestRef.current !== requestId) {
              return;
            }

            // Keep prior analytics values on transport failures.
            if (error instanceof NetcodeAnalyticsError) {
              const rollout = error.payload.rollout;
              const isRolloutDisabled = rollout?.enabled === false ||
                  (error.payload.error ?? error.message)
                      .toLowerCase()
                      .includes('rollout disabled');

              setAnalyticsAvailability({
                available: false,
                reason: isRolloutDisabled ? 'rollout-disabled' :
                                            'contract-error',
                errorCode: error.payload.errorCode,
                retryable: error.payload.retryable,
                contractVersion: error.payload.contractVersion,
                rollout: {
                  enabled: rollout?.enabled ?? !isRolloutDisabled,
                  cohort: rollout?.cohort ?? 'default',
                },
              });
              return;
            }

            setAnalyticsAvailability({
              available: false,
              reason: 'transport-error',
              rollout: {
                enabled: true,
                cohort: 'default',
              },
            });
          }
        };

        void run();
      },
      [
        branchPressure,
        callDensity,
        comboStreak,
        dependencyPulse,
        learningModel.modelConfidence,
        learningModel.policyMomentum,
        networkDimensions,
        playerLevel,
        questPercent,
        workflowDepth,
      ]);

  return {
    learningModel,
    ledger,
    recordNodeTap,
    recordAction,
    rewardSignal,
    linkConfidence,
    contractVectors,
    hypersphereProjection,
    k3h4,
    analyticsAvailability,
  };
}
