import type {NativeNetcodeService, NetcodeHypersphereOutput, NetcodeLinkOutput, NetcodeRewardOutput, NetcodeVectorOutput,} from './nativeNetcode.ts';

export type NetcodeHypersphereRollout = {
  enabled: boolean; cohort: string;
};

export type NetcodeAbiLayerName =
    'learning'|'reward'|'link'|'vector'|'hypersphere';

export type NetcodeAbiLayerSnapshot = {
  layer: NetcodeAbiLayerName;
  contractVersion: 1;
  status: 'ok'|'unsupported-version'|'invalid-payload'|'nonfinite-value'|
      'crc-mismatch';
  payloadBytes: number;
  byteOrderTag: number;
  deterministicHash: number;
};

export type NetcodeAnalyticsAuthoritativeRequest = {
  callDensity: number; questPercent: number; playerLevel: number;
  comboStreak: number;
  branchPressure: number;
  dependencyPulse: number;
  workflowDepth: 1 | 2 | 3;
  networkDimensions: number;
  modelConfidence: number;
  policyMomentum: number;
  interactionSignal?: number;
};

export type NetcodeHypersphereKmeansProjection = {
  centers: NetcodeHypersphereOutput['centers'];
  radii: NetcodeHypersphereOutput['radii'];
  weightedVoronoiScores: NetcodeHypersphereOutput['weightedVoronoiScores'];
  spectralProxy: NetcodeHypersphereOutput['spectralProxy'];
  observability: NetcodeHypersphereOutput['observability'];
};

export type NetcodeLspRepresentation = {
  language: 'netcode.analytics.v1'; boundedContext: 'netcode';
  aggregate: 'k3h4';
  authority: 'server-native';
  contractVersion: 1;
  deterministicHash: number;
  rollout: NetcodeHypersphereRollout;
};

export type NetcodeAnalyticsAuthoritativeResult = {
  contractVersion: 1; reward: NetcodeRewardOutput; link: NetcodeLinkOutput;
  vector: NetcodeVectorOutput;
  hypersphere: NetcodeHypersphereOutput;
  k3h4: NetcodeHypersphereKmeansProjection;
  abiLayers: readonly NetcodeAbiLayerSnapshot[];
  lspRepresentation: NetcodeLspRepresentation;
};

export class NetcodeAnalyticsOrchestrationError extends Error {
  readonly errorCode: 'ERR_UNSUPPORTED_VERSION'|'ERR_BAD_CRC'|
      'ERR_PAYLOAD_TRUNCATED';
  readonly retryable: boolean;

  constructor(
      errorCode: 'ERR_UNSUPPORTED_VERSION'|'ERR_BAD_CRC'|
      'ERR_PAYLOAD_TRUNCATED',
      message: string,
      retryable: boolean,
  ) {
    super(message);
    this.name = 'NetcodeAnalyticsOrchestrationError';
    this.errorCode = errorCode;
    this.retryable = retryable;
  }
}

export interface NetcodeAnalyticsAuthoritativeComputeOrchestrator {
  compute(
      request: NetcodeAnalyticsAuthoritativeRequest,
      rollout: NetcodeHypersphereRollout,
      ): Promise<NetcodeAnalyticsAuthoritativeResult>;
}

function mapHypersphereBuildError(error: unknown):
    NetcodeAnalyticsOrchestrationError|undefined {
  const message =
      error instanceof Error ? error.message : 'Unknown hypersphere error';

  if (message.includes('Unsupported native hypersphere contract version')) {
    return new NetcodeAnalyticsOrchestrationError(
        'ERR_UNSUPPORTED_VERSION', message, false);
  }

  if (message.includes('CRC mismatch')) {
    return new NetcodeAnalyticsOrchestrationError('ERR_BAD_CRC', message, true);
  }

  if (message.includes('truncated payload')) {
    return new NetcodeAnalyticsOrchestrationError(
        'ERR_PAYLOAD_TRUNCATED', message, true);
  }

  return undefined;
}

function resolveRewardInteractionSignal(
    request: NetcodeAnalyticsAuthoritativeRequest,
    ): number {
  if (typeof request.interactionSignal === 'number') {
    return request.interactionSignal;
  }

  return Math.round((request.modelConfidence * 2 + request.policyMomentum) / 3);
}

function buildAbiLayerCatalog(
    reward: NetcodeRewardOutput,
    link: NetcodeLinkOutput,
    vector: NetcodeVectorOutput,
    hypersphere: NetcodeHypersphereOutput,
    ): readonly NetcodeAbiLayerSnapshot[] {
  const hypersphereEnvelope = hypersphere.envelope;
  return [
    {
      layer: 'learning',
      contractVersion: 1,
      status: 'ok',
      payloadBytes: 0,
      byteOrderTag: 0,
      deterministicHash: reward.neuralRelevanceScore,
    },
    {
      layer: 'reward',
      contractVersion: 1,
      status: 'ok',
      payloadBytes: 0,
      byteOrderTag: 0,
      deterministicHash: reward.projectedRewardXp,
    },
    {
      layer: 'link',
      contractVersion: 1,
      status: 'ok',
      payloadBytes: 0,
      byteOrderTag: 0,
      deterministicHash: link.intel + link.objectives + link.player + link.ops,
    },
    {
      layer: 'vector',
      contractVersion: 1,
      status: 'ok',
      payloadBytes: 0,
      byteOrderTag: 0,
      deterministicHash: vector.dimensions,
    },
    {
      layer: 'hypersphere',
      contractVersion: 1,
      status: hypersphereEnvelope?.status ?? 'ok',
      payloadBytes: hypersphereEnvelope?.payloadBytes ?? 0,
      byteOrderTag: hypersphereEnvelope?.byteOrderTag ?? 0,
      deterministicHash: hypersphere.observability.deterministicHash,
    },
  ];
}

class NativeNetcodeAuthoritativeComputeOrchestrator implements
    NetcodeAnalyticsAuthoritativeComputeOrchestrator {
  constructor(private readonly netcode: NativeNetcodeService) {}

  public async compute(
      request: NetcodeAnalyticsAuthoritativeRequest,
      rollout: NetcodeHypersphereRollout,
      ): Promise<NetcodeAnalyticsAuthoritativeResult> {
    const reward = await this.netcode.buildReward(
        {
          callDensity: request.callDensity,
          questPercent: request.questPercent,
          comboStreak: request.comboStreak,
          branchPressure: request.branchPressure,
          workflowDepth: request.workflowDepth,
        },
        resolveRewardInteractionSignal(request),
    );

    const link = await this.netcode.buildLink({
      callDensity: request.callDensity,
      questPercent: request.questPercent,
      playerLevel: request.playerLevel,
      comboStreak: request.comboStreak,
      branchPressure: request.branchPressure,
      dependencyPulse: request.dependencyPulse,
      interactionSignal: request.policyMomentum,
    });

    const vectorInput = {
      callDensity: request.callDensity,
      questPercent: request.questPercent,
      playerLevel: request.playerLevel,
      comboStreak: request.comboStreak,
      branchPressure: request.branchPressure,
      dependencyPulse: request.dependencyPulse,
      workflowDepth: request.workflowDepth,
      neuralRelevanceScore: reward.neuralRelevanceScore,
      networkDimensions: request.networkDimensions,
      modelConfidence: request.modelConfidence,
      policyMomentum: request.policyMomentum,
    };

    const vector = await this.netcode.buildVector(vectorInput);

    let hypersphere: NetcodeHypersphereOutput;
    try {
      hypersphere = await this.netcode.buildHypersphere(vectorInput);
    } catch (error) {
      const mappedError = mapHypersphereBuildError(error);
      if (mappedError) {
        throw mappedError;
      }
      throw error;
    }

    const k3h4 = {
      centers: hypersphere.centers,
      radii: hypersphere.radii,
      weightedVoronoiScores: hypersphere.weightedVoronoiScores,
      spectralProxy: hypersphere.spectralProxy,
      observability: hypersphere.observability,
    };
    const abiLayers = buildAbiLayerCatalog(reward, link, vector, hypersphere);

    return {
      contractVersion: 1,
      reward,
      link,
      vector,
      hypersphere,
      k3h4,
      abiLayers,
      lspRepresentation: {
        language: 'netcode.analytics.v1',
        boundedContext: 'netcode',
        aggregate: 'k3h4',
        authority: 'server-native',
        contractVersion: 1,
        deterministicHash: hypersphere.observability.deterministicHash,
        rollout,
      },
    };
  }
}

export function createNetcodeAnalyticsAuthoritativeComputeOrchestrator(
    netcode: NativeNetcodeService,
    ): NetcodeAnalyticsAuthoritativeComputeOrchestrator {
  return new NativeNetcodeAuthoritativeComputeOrchestrator(netcode);
}
