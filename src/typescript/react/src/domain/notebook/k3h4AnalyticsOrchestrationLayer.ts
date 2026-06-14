import {type NetcodeAbiLayerCoverage, type NetcodeAbiLayerLedger} from '@banana/ui';

import type {NetcodeAnalyticsAbiLayerSnapshot, NetcodeAnalyticsResponse,} from '../../lib/api';

import type {ContractK3h4Model, ContractK3h4ProjectionModel, ContractNodeId, ContractNodeVectorModel, NodeLinkConfidenceModel, RewardSignalModel,} from './network-domain';

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
  k3h4Projection: ContractK3h4ProjectionModel;
  k3h4: ContractK3h4Model;
  abiLayers: readonly NetcodeAnalyticsAbiLayerSnapshot[];
  abiLayerCoverage: NetcodeAbiLayerCoverage;
  abiLayerLedger: NetcodeAbiLayerLedger;
};

function normalizeConvergenceStatus(value: unknown): 'converged'|
    'max-iterations'|'failed' {
  if (value === 'converged' || value === 'max-iterations' ||
      value === 'failed') {
    return value;
  }

  if (value === 0) return 'converged';
  if (value === 1) return 'max-iterations';
  return 'failed';
}

function normalizeScoringValidity(value: unknown): 'valid'|'degraded'|
    'invalid' {
  if (value === 'valid' || value === 'degraded' || value === 'invalid') {
    return value;
  }

  if (value === 0) return 'valid';
  if (value === 1) return 'degraded';
  return 'invalid';
}

function normalizeDeterministicHash(value: unknown): string {
  if (typeof value === 'string' && value.trim().length > 0) {
    return value.trim();
  }

  if (typeof value === 'number' && Number.isFinite(value)) {
    return (value >>> 0).toString(16);
  }

  return '';
}

function normalizeMemberVectorIds(
    memberVectorIds: unknown, memberCount: number): number[] {
  if (Array.isArray(memberVectorIds) && memberVectorIds.length > 0) {
    return memberVectorIds.filter((value) => Number.isFinite(value));
  }

  return Array.from({length: Math.max(0, memberCount)}, (_, index) => index);
}

export function mapK3h4AnalyticsToPresentationState(
    analytics: NetcodeAnalyticsResponse,
    ): K3h4AnalyticsPresentationState {
  const safeAbiCoverage = analytics.abiLayerCoverage ?? {
    expectedLayers: [],
    presentLayers: [],
    missingLayers: [],
    completeness: 0,
    complete: false,
  };
  const safeAbiLedger = analytics.abiLayerLedger ?? [];
  const safeNodeVectors = analytics.vector?.nodeVectors ?? [];
  const safeContractStrength = analytics.vector?.contractStrength ?? [];
  const safeProjectionNodes = analytics.k3h4Projection?.nodes ?? [];
  const safeK3h4 = analytics.k3h4 ?? {
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

  const rewardTier = analytics.reward.rewardTier === 0 ? 'Elite Signal' :
      analytics.reward.rewardTier === 1                ? 'Relevant' :
                                                         'Needs Labeling';
  const rewardBand = rewardTier === 'Elite Signal' ? ELITE_BAND :
      rewardTier === 'Relevant'                    ? RELEVANT_BAND :
                                                     LABELING_BAND;

  const contractVectors = NODE_ORDER.map(
      (id, index) => ({
        id,
        dimensions: [...(safeNodeVectors[index] ?? [])],
        contractStrength: safeContractStrength[index] ?? 0,
      }));

  const k3h4Projection = {
    dimensions: analytics.k3h4Projection?.dimensions ?? 0,
    nodes: NODE_ORDER.map(
        (id, index) => ({
          id,
          x: safeProjectionNodes[index]?.x ?? 0,
          y: safeProjectionNodes[index]?.y ?? 0,
          z: safeProjectionNodes[index]?.z ?? 0,
          coherence: safeProjectionNodes[index]?.coherence ?? 0,
          inradius: safeProjectionNodes[index]?.inradius ?? 0,
          nearestNeighborDistance:
              safeProjectionNodes[index]?.nearestNeighborDistance ??
              0,
        })),
    alignment: analytics.k3h4Projection?.alignment ?? 0,
    radialStability: analytics.k3h4Projection?.radialStability ?? 0,
  };

  const k3h4 = {
    centers: (safeK3h4.centers ?? []).map((center) => {
      const normalizedCenter = center as {
        readonly memberVectorIds?: readonly number[];
        readonly memberCount: number;
      };

      return {
        clusterId: center.clusterId,
        centerQ16: [...center.centerQ16],
        memberVectorIds: normalizeMemberVectorIds(
            normalizedCenter.memberVectorIds, normalizedCenter.memberCount),
        memberCount: center.memberCount,
      };
    }),
    radii: (safeK3h4.radii ?? []).map(
        (radius) => ({
          clusterId: radius.clusterId,
          nearestNeighborDistanceQ16: radius.nearestNeighborDistanceQ16,
          inscribedRadiusQ16: radius.inscribedRadiusQ16,
          radiusState: radius.radiusState,
        })),
    weightedVoronoiScores: (safeK3h4.weightedVoronoiScores ?? []).map(
        (score) => ({
          vectorId: score.vectorId,
          clusterId: score.clusterId,
          distanceToCenterQ16: score.distanceToCenterQ16,
          weightedScoreQ16: score.weightedScoreQ16,
          scoreValidity: score.scoreValidity,
        })),
    spectralProxy: (safeK3h4.spectralProxy ?? []).map(
        (entry) => ({
          clusterId: entry.clusterId,
          frequencyProxyQ16: entry.frequencyProxyQ16,
          amplitudeProxyQ16: entry.amplitudeProxyQ16,
          spectralState: entry.spectralState,
        })),
    observability: {
      convergenceStatus: normalizeConvergenceStatus(
          safeK3h4.observability?.convergenceStatus),
      iterationCount: safeK3h4.observability?.iterationCount ?? 0,
      assignmentChangesLastIteration:
          safeK3h4.observability?.assignmentChangesLastIteration ?? 0,
      scoringValidity:
          normalizeScoringValidity((safeK3h4.observability as {
                                     readonly scoringValidity?: unknown;
                                   }).scoringValidity),
      deterministicHash: normalizeDeterministicHash(
          safeK3h4.observability?.deterministicHash),
    },
    runtime: {
      mode: analytics.k3h4Runtime?.mode ?? 'multiplicative',
      spectralActivation:
          analytics.k3h4Runtime?.spectralActivation ?? 'disabled',
    },
  };

  const abiLayers = (analytics.abiLayers ?? []).map((layer) => ({...layer}));
  const abiLayerCoverage = {
    expectedLayers: [...safeAbiCoverage.expectedLayers],
    presentLayers: [...safeAbiCoverage.presentLayers],
    missingLayers: [...safeAbiCoverage.missingLayers],
    completeness: safeAbiCoverage.completeness,
    complete: safeAbiCoverage.complete,
  };
  const abiLayerLedger = safeAbiLedger.map((layer) => ({...layer}));

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
    k3h4Projection,
    k3h4,
    abiLayers,
    abiLayerCoverage,
    abiLayerLedger,
  };
}
