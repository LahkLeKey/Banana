import type {NetcodeAnalyticsAbiLayerSnapshot, NetcodeAnalyticsResponse,} from '../../lib/api';

import type {ContractHypersphereKmeansModel, ContractHypersphereProjectionModel, ContractNodeId, ContractNodeVectorModel, NodeLinkConfidenceModel, RewardSignalModel,} from './network-domain';

const NODE_ORDER: ContractNodeId[] = ['intel', 'objectives', 'player', 'ops'];

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

export type K3h4AnalyticsPresentationState = {
  rewardSignal: RewardSignalModel; linkConfidence: NodeLinkConfidenceModel;
  contractVectors: readonly ContractNodeVectorModel[];
  hypersphereProjection: ContractHypersphereProjectionModel;
  k3h4: ContractHypersphereKmeansModel;
  abiLayers: readonly NetcodeAnalyticsAbiLayerSnapshot[];
};

export function mapK3h4AnalyticsToPresentationState(
    analytics: NetcodeAnalyticsResponse,
    ): K3h4AnalyticsPresentationState {
  const rewardTier = analytics.reward.rewardTier === 0 ? 'Elite Signal' :
      analytics.reward.rewardTier === 1                ? 'Relevant' :
                                                         'Needs Labeling';
  const rewardBand = rewardTier === 'Elite Signal' ? ELITE_BAND :
      rewardTier === 'Relevant'                    ? RELEVANT_BAND :
                                                     LABELING_BAND;

  const contractVectors = NODE_ORDER.map(
      (id, index) => ({
        id,
        dimensions: [...(analytics.vector.nodeVectors[index] ?? [])],
        contractStrength: analytics.vector.contractStrength[index] ?? 0,
      }));

  const hypersphereProjection = {
    dimensions: analytics.hypersphere.dimensions,
    nodes: NODE_ORDER.map(
        (id, index) => ({
          id,
          x: analytics.hypersphere.nodes[index]?.x ?? 0,
          y: analytics.hypersphere.nodes[index]?.y ?? 0,
          z: analytics.hypersphere.nodes[index]?.z ?? 0,
          coherence: analytics.hypersphere.nodes[index]?.coherence ?? 0,
          inradius: analytics.hypersphere.nodes[index]?.inradius ?? 0,
          nearestNeighborDistance:
              analytics.hypersphere.nodes[index]?.nearestNeighborDistance ?? 0,
        })),
    alignment: analytics.hypersphere.alignment,
    radialStability: analytics.hypersphere.radialStability,
  };

  const k3h4 = {
    centers: analytics.k3h4.centers.map(
        (center) => ({
          clusterId: center.clusterId,
          centerQ16: [...center.centerQ16],
          memberVectorIds: [...center.memberVectorIds],
          memberCount: center.memberCount,
        })),
    radii: analytics.k3h4.radii.map(
        (radius) => ({
          clusterId: radius.clusterId,
          nearestNeighborDistanceQ16: radius.nearestNeighborDistanceQ16,
          inscribedRadiusQ16: radius.inscribedRadiusQ16,
          radiusState: radius.radiusState,
        })),
    weightedVoronoiScores: analytics.k3h4.weightedVoronoiScores.map(
        (score) => ({
          vectorId: score.vectorId,
          clusterId: score.clusterId,
          distanceToCenterQ16: score.distanceToCenterQ16,
          weightedScoreQ16: score.weightedScoreQ16,
          scoreValidity: score.scoreValidity,
        })),
    spectralProxy: analytics.k3h4.spectralProxy.map(
        (entry) => ({
          clusterId: entry.clusterId,
          frequencyProxyQ16: entry.frequencyProxyQ16,
          amplitudeProxyQ16: entry.amplitudeProxyQ16,
          spectralState: entry.spectralState,
        })),
    observability: {
      convergenceStatus: analytics.k3h4.observability.convergenceStatus,
      iterationCount: analytics.k3h4.observability.iterationCount,
      assignmentChangesLastIteration:
          analytics.k3h4.observability.assignmentChangesLastIteration,
      scoringValidity: analytics.k3h4.observability.scoringValidity,
      deterministicHash: analytics.k3h4.observability.deterministicHash,
    },
  };

  return {
    rewardSignal: {
      neuralRelevanceScore: analytics.reward.neuralRelevanceScore,
      projectedRewardXp: analytics.reward.projectedRewardXp,
      rewardTier,
      rewardBand,
    },
    linkConfidence: {
      intel: analytics.link.intel,
      objectives: analytics.link.objectives,
      player: analytics.link.player,
      ops: analytics.link.ops,
    },
    contractVectors,
    hypersphereProjection,
    k3h4,
    abiLayers: analytics.abiLayers.map((layer) => ({...layer})),
  };
}
