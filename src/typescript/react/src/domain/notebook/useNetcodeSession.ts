import {useCallback, useEffect, useRef, useState} from 'react';

import {fetchNetcodeAnalytics, fetchNetcodeLearning, resolveApiBaseUrl,} from '../../lib/api';

import type {ContractHypersphereProjectionModel, ContractNodeId, ContractNodeVectorModel, NodeInteractionAction, NodeInteractionLearningModel, NodeInteractionLedger, NodeLinkConfidenceModel, RewardSignalModel,} from './network-domain';

type NetcodeSignals = {
  readonly callDensity: number; readonly questPercent: number; readonly comboStreak: number; readonly branchPressure: number; readonly workflowDepth: 1 | 2 | 3; readonly playerLevel: number; readonly dependencyPulse: number; readonly networkDimensions:
                                                                                                                                                                                                                                              number;
};

const ELITE_BAND = {
  line: '#22c55e',
  soft: 'rgba(34, 197, 94, 0.28)',
  glow: 'rgba(34, 197, 94, 0.42)',
} as const;

const RELEVANT_BAND = {
  line: '#f59e0b',
  soft: 'rgba(245, 158, 11, 0.26)',
  glow: 'rgba(245, 158, 11, 0.4)',
} as const;

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
  return {id, x: 0, y: 0, z: 0, coherence: 0};
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

const NODE_ORDER: ContractNodeId[] = ['intel', 'objectives', 'player', 'ops'];

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
  readonly learningModel: NodeInteractionLearningModel; readonly ledger: NodeInteractionLedger; readonly recordNodeTap: (node: ContractNodeId) => void; readonly recordAction: (action: NodeInteractionAction) => void; readonly rewardSignal: RewardSignalModel; readonly linkConfidence: NodeLinkConfidenceModel; readonly contractVectors: readonly ContractNodeVectorModel[]; readonly hypersphereProjection:
                                                                                                                                                                                                                                                                                                                                                                                               ContractHypersphereProjectionModel;
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

            const rewardTier = analytics.reward.rewardTier === 0 ?
                'Elite Signal' :
                analytics.reward.rewardTier === 1 ? 'Relevant' :
                                                    'Needs Labeling';
            const rewardBand = rewardTier === 'Elite Signal' ? ELITE_BAND :
                rewardTier === 'Relevant'                    ? RELEVANT_BAND :
                                                               LABELING_BAND;
            setRewardSignal({
              neuralRelevanceScore: analytics.reward.neuralRelevanceScore,
              projectedRewardXp: analytics.reward.projectedRewardXp,
              rewardTier,
              rewardBand,
            });

            setLinkConfidence({
              intel: analytics.link.intel,
              objectives: analytics.link.objectives,
              player: analytics.link.player,
              ops: analytics.link.ops,
            });

            const mappedVectors = NODE_ORDER.map(
                (id, index) => ({
                  id,
                  dimensions: [...(analytics.vector.nodeVectors[index] ?? [])],
                  contractStrength:
                      analytics.vector.contractStrength[index] ?? 0,
                }));
            setContractVectors(mappedVectors);

            const mappedNodes = NODE_ORDER.map(
                (id, index) => ({
                  id,
                  x: analytics.hypersphere.nodes[index]?.x ?? 0,
                  y: analytics.hypersphere.nodes[index]?.y ?? 0,
                  z: analytics.hypersphere.nodes[index]?.z ?? 0,
                  coherence: analytics.hypersphere.nodes[index]?.coherence ?? 0,
                }));
            setHypersphereProjection({
              dimensions: analytics.hypersphere.dimensions,
              nodes: mappedNodes,
              alignment: analytics.hypersphere.alignment,
              radialStability: analytics.hypersphere.radialStability,
            });
          } catch {
            // Keep prior analytics values on transport failures.
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
  };
}
