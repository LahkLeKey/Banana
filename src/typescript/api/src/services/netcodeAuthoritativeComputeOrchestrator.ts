import type {NativeNetcodeService, NetcodeK3h4Output, NetcodeLinkOutput, NetcodeRewardOutput, NetcodeVectorOutput,} from './nativeNetcode.ts';

export type NetcodeK3h4Rollout = {
  enabled: boolean; cohort: string;
};

export type NetcodeAbiLayerName = 'learning'|'reward'|'link'|'vector'|'k3h4';

/**
 * Snapshot of one native ABI layer as surfaced to API consumers.
 *
 * The first four layers are logical checkpoints in the authoritative compute
 * pipeline; the k3h4 layer additionally carries native envelope metadata.
 */
export type NetcodeAbiLayerSnapshot = {
  layer: NetcodeAbiLayerName; contractVersion: 1;
  status: 'ok' | 'unsupported-version' | 'invalid-payload' | 'nonfinite-value' |
      'crc-mismatch';
  payloadBytes: number;
  byteOrderTag: number;
  deterministicHash: number;
};

const NETCODE_ABI_LAYER_ORDER: NetcodeAbiLayerName[] =
    ['learning', 'reward', 'link', 'vector', 'k3h4'];

export type NetcodeAbiLayerCoverage = {
  expectedLayers: readonly NetcodeAbiLayerName[];
  presentLayers: readonly NetcodeAbiLayerName[];
  missingLayers: readonly NetcodeAbiLayerName[];
  completeness: number;
  complete: boolean;
};

export type NetcodeAbiLayerDatum = {
  layer: NetcodeAbiLayerName; present: boolean; contractVersion: number;
  status: NetcodeAbiLayerSnapshot['status'] | 'missing';
  payloadBytes: number;
  byteOrderTag: number;
  deterministicHash: number | null;
};

export type NetcodeAbiLayerLedger = readonly NetcodeAbiLayerDatum[];

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
  k3h4Mode?: 'multiplicative' | 'power';
  spectralMode?: 'disabled' | 'affinity-graph';
};

export type NetcodeK3h4RuntimeMetadata = {
  mode: 'multiplicative'|'power';
  spectralActivation: 'disabled' | 'affinity-graph';
};

/**
 * API-facing subset of the native k3h4 payload that callers need for runtime
 * inspection and UI projection without repeating the full response shape.
 */
export type NetcodeK3h4Projection = {
  centers: NetcodeK3h4Output['centers']; radii: NetcodeK3h4Output['radii'];
  weightedVoronoiScores: NetcodeK3h4Output['weightedVoronoiScores'];
  spectralProxy: NetcodeK3h4Output['spectralProxy'];
  observability: NetcodeK3h4Output['observability'];
};

export type NetcodeLspRepresentation = {
  language: 'netcode.analytics.v1'; boundedContext: 'netcode';
  aggregate: 'k3h4';
  authority: 'server-native';
  contractVersion: 1;
  deterministicHash: number;
  rollout: NetcodeK3h4Rollout;
};

export type NetcodeAnalyticsAuthoritativeResult = {
  contractVersion: 1; reward: NetcodeRewardOutput; link: NetcodeLinkOutput;
  vector: NetcodeVectorOutput;
  k3h4Projection: NetcodeK3h4Output;
  k3h4: NetcodeK3h4Projection;
  k3h4Runtime: NetcodeK3h4RuntimeMetadata;
  abiLayers: readonly NetcodeAbiLayerSnapshot[];
  abiLayerCoverage: NetcodeAbiLayerCoverage;
  abiLayerLedger: NetcodeAbiLayerLedger;
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
  /**
   * Runs the reward -> link -> vector -> k3h4 pipeline against the native
   * service and returns projection data plus ABI observability metadata.
   */
  compute(
      request: NetcodeAnalyticsAuthoritativeRequest,
      rollout: NetcodeK3h4Rollout,
      ): Promise<NetcodeAnalyticsAuthoritativeResult>;
}

function mapK3h4BuildError(error: unknown): NetcodeAnalyticsOrchestrationError|
    undefined {
  const message = error instanceof Error ? error.message : 'Unknown k3h4 error';

  if (message.includes('Unsupported native k3h4 contract version')) {
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
    k3h4Projection: NetcodeK3h4Output,
    ): readonly NetcodeAbiLayerSnapshot[] {
  const k3h4Envelope = k3h4Projection.envelope;
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
      layer: 'k3h4',
      contractVersion: 1,
      status: k3h4Envelope?.status ?? 'ok',
      payloadBytes: k3h4Envelope?.payloadBytes ?? 0,
      byteOrderTag: k3h4Envelope?.byteOrderTag ?? 0,
      deterministicHash: k3h4Projection.observability.deterministicHash,
    },
  ];
}

function summarizeAbiLayerCoverage(
    abiLayers: readonly NetcodeAbiLayerSnapshot[],
    ): NetcodeAbiLayerCoverage {
  const presentLayers = new Set<NetcodeAbiLayerName>();
  for (const layer of abiLayers) {
    presentLayers.add(layer.layer);
  }

  const presentLayerList =
      NETCODE_ABI_LAYER_ORDER.filter((layer) => presentLayers.has(layer));
  const missingLayers =
      NETCODE_ABI_LAYER_ORDER.filter((layer) => !presentLayers.has(layer));

  return {
    expectedLayers: NETCODE_ABI_LAYER_ORDER,
    presentLayers: presentLayerList,
    missingLayers,
    completeness: NETCODE_ABI_LAYER_ORDER.length === 0 ?
        1 :
        presentLayerList.length / NETCODE_ABI_LAYER_ORDER.length,
    complete: missingLayers.length === 0,
  };
}

function buildAbiLayerLedger(
    abiLayers: readonly NetcodeAbiLayerSnapshot[],
    ): NetcodeAbiLayerLedger {
  const layerMap = new Map<NetcodeAbiLayerName, NetcodeAbiLayerSnapshot>();
  for (const layer of abiLayers) {
    if (!layerMap.has(layer.layer)) {
      layerMap.set(layer.layer, layer);
    }
  }

  return NETCODE_ABI_LAYER_ORDER.map((layer) => {
    const snapshot = layerMap.get(layer);
    if (!snapshot) {
      return {
        layer,
        present: false,
        contractVersion: 1,
        status: 'missing',
        payloadBytes: 0,
        byteOrderTag: 0,
        deterministicHash: null,
      };
    }

    return {
      layer,
      present: true,
      contractVersion: snapshot.contractVersion,
      status: snapshot.status,
      payloadBytes: snapshot.payloadBytes,
      byteOrderTag: snapshot.byteOrderTag,
      deterministicHash: snapshot.deterministicHash,
    };
  });
}

class NativeNetcodeAuthoritativeComputeOrchestrator implements
    NetcodeAnalyticsAuthoritativeComputeOrchestrator {
  constructor(private readonly netcode: NativeNetcodeService) {}

  public async compute(
      request: NetcodeAnalyticsAuthoritativeRequest,
      rollout: NetcodeK3h4Rollout,
      ): Promise<NetcodeAnalyticsAuthoritativeResult> {
    // Keep the API pipeline order aligned with the native authoritative path so
    // ABI layer coverage and deterministic hashes stay comparable across
    // layers.
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
      k3h4Mode: request.k3h4Mode ?? 'multiplicative',
      spectralMode: request.spectralMode ?? 'disabled',
    };

    const vector = await this.netcode.buildVector(vectorInput);

    let k3h4Projection: NetcodeK3h4Output;
    try {
      k3h4Projection = await this.netcode.buildK3h4(vectorInput);
    } catch (error) {
      const mappedError = mapK3h4BuildError(error);
      if (mappedError) {
        throw mappedError;
      }
      throw error;
    }

    const k3h4 = {
      centers: k3h4Projection.centers,
      radii: k3h4Projection.radii,
      weightedVoronoiScores: k3h4Projection.weightedVoronoiScores,
      spectralProxy: k3h4Projection.spectralProxy,
      observability: k3h4Projection.observability,
    };
    const k3h4Runtime: NetcodeK3h4RuntimeMetadata = {
      mode: vectorInput.k3h4Mode,
      spectralActivation: vectorInput.spectralMode,
    };
    const abiLayers =
        buildAbiLayerCatalog(reward, link, vector, k3h4Projection);
    const abiLayerCoverage = summarizeAbiLayerCoverage(abiLayers);
    const abiLayerLedger = buildAbiLayerLedger(abiLayers);

    return {
      contractVersion: 1,
      reward,
      link,
      vector,
      k3h4Projection,
      k3h4,
      k3h4Runtime,
      abiLayers,
      abiLayerCoverage,
      abiLayerLedger,
      lspRepresentation: {
        language: 'netcode.analytics.v1',
        boundedContext: 'netcode',
        aggregate: 'k3h4',
        authority: 'server-native',
        contractVersion: 1,
        deterministicHash: k3h4Projection.observability.deterministicHash,
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
