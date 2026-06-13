import {FFIType, type Pointer, ptr} from 'bun:ffi';

import {loadBananaNativeSymbols,} from '../lib/native-interop-loader.ts';

export type NetcodeSignalInput = {
  readonly callDensity: number; readonly questPercent: number; readonly comboStreak: number; readonly branchPressure: number; readonly workflowDepth:
                                                                                                                                           1 |
      2 | 3;
};

export type NetcodeLedger = {
  readonly snapshots: number; readonly inspections: number; readonly trainings: number; readonly routes: number; readonly nodeTaps:
                                                                                                                              number;
};

export type NetcodeLearningOutput =
    {
      readonly modelConfidence: number; readonly trainingAccuracy: number; readonly policyMomentum: number; readonly nodeWeights: readonly[number, number, number, number]; readonly recommendedNode: number; readonly recommendedAction: number; readonly xpByAction: readonly[number, number, number, number];
    };

export type NetcodeRewardOutput = {
  readonly neuralRelevanceScore: number; readonly projectedRewardXp: number; readonly rewardTier:
                                                                                          number;
};

export type NetcodeLinkInput = {
  readonly callDensity: number; readonly questPercent: number; readonly playerLevel: number; readonly comboStreak: number; readonly branchPressure: number; readonly dependencyPulse: number; readonly interactionSignal:
                                                                                                                                                                                                           number;
};

export type NetcodeLinkOutput = {
  readonly intel: number; readonly objectives: number; readonly player: number; readonly ops:
                                                                                             number;
};

export type NetcodeVectorInput =
    {
      readonly callDensity: number; readonly questPercent: number; readonly playerLevel: number; readonly comboStreak: number; readonly branchPressure: number; readonly dependencyPulse: number; readonly workflowDepth: 1 | 2 | 3; readonly neuralRelevanceScore: number; readonly networkDimensions: number; readonly modelConfidence: number; readonly policyMomentum:
                                                                                                                                                                                                                                                                                                                                                               number;
    };

export type NetcodeVectorOutput = {
  readonly dimensions: number; readonly nodeVectors: readonly(readonly number[])[]; readonly contractStrength:
                                                                                                 readonly[number, number, number, number];
};

export type NetcodeHypersphereKmeansContractStatus = 'ok'|'unsupported-version'|
    'invalid-payload'|'nonfinite-value'|'crc-mismatch';

export type NetcodeHypersphereKmeansEnvelope = {
  readonly contractVersion: number; readonly byteOrderTag: number; readonly payloadBytes: number; readonly payloadCrc32: number; readonly status:
                                                                                                                                              NetcodeHypersphereKmeansContractStatus;
};

export type NetcodeHypersphereNode = {
  readonly x: number; readonly y: number; readonly z: number; readonly coherence: number; readonly inradius: number; readonly nearestNeighborDistance:
                                                                                                                                  number;
};

export type NetcodeHypersphereRadiusState =
    'ok'|'single-cluster'|'near-zero-clamped';

export type NetcodeHypersphereScoreValidity = 'valid'|'invalid-radius';

export type NetcodeHypersphereSpectralState = 'ok'|'radius-floor-applied';

export type NetcodeHypersphereEndiannessDecodePath =
    'little-endian'|'byte-swapped';

type NetcodeQ16Vector = readonly number[];

export type NetcodeHypersphereKmeansCenter = {
  readonly clusterId: number;
  readonly memberCount: number;
  readonly centerQ16: NetcodeQ16Vector;
};

export type NetcodeHypersphereKmeansRadius = {
  readonly clusterId: number; readonly nearestNeighborDistanceQ16: number; readonly inscribedRadiusQ16: number; readonly radiusState:
                                                                                                                             NetcodeHypersphereRadiusState;
};

export type NetcodeHypersphereWeightedVoronoiScore = {
  readonly vectorId: number; readonly clusterId: number; readonly distanceToCenterQ16: number; readonly weightedScoreQ16: number; readonly scoreValidity:
                                                                                                                                               NetcodeHypersphereScoreValidity;
};

export type NetcodeHypersphereSpectralProxy = {
  readonly clusterId: number; readonly frequencyProxyQ16: number; readonly amplitudeProxyQ16: number; readonly spectralState:
                                                                                                                   NetcodeHypersphereSpectralState;
};

export type NetcodeHypersphereObservability = {
  readonly convergenceStatus: number; readonly iterationCount: number; readonly assignmentChangesLastIteration: number; readonly deterministicHash: number; readonly endiannessDecodePath:
                                                                                                                                                                         NetcodeHypersphereEndiannessDecodePath;
};

export type NetcodeHypersphereOutput = {
  readonly dimensions: number;
  readonly nodes: readonly [
    NetcodeHypersphereNode,
    NetcodeHypersphereNode,
    NetcodeHypersphereNode,
    NetcodeHypersphereNode,
  ];
  readonly alignment: number;
  readonly radialStability: number;
  readonly clusterCount: number;
  readonly vectorCount: number;
  readonly centers: readonly NetcodeHypersphereKmeansCenter[];
  readonly radii: readonly NetcodeHypersphereKmeansRadius[];
  readonly weightedVoronoiScores: readonly NetcodeHypersphereWeightedVoronoiScore[];
  readonly spectralProxy: readonly NetcodeHypersphereSpectralProxy[];
  readonly observability: NetcodeHypersphereObservability;
  readonly envelope?: NetcodeHypersphereKmeansEnvelope;
};

const BANANA_NETCODE_HYPERSPHERE_PAYLOAD_BYTES = 872;
const BANANA_NETCODE_HYPERSPHERE_ENVELOPE_BYTES = 20;
const BANANA_NETCODE_HYPERSPHERE_BUFFER_BYTES =
    BANANA_NETCODE_HYPERSPHERE_PAYLOAD_BYTES +
    BANANA_NETCODE_HYPERSPHERE_ENVELOPE_BYTES;

function mapRadiusState(value: number): NetcodeHypersphereRadiusState {
  if (value === 1) return 'single-cluster';
  if (value === 2) return 'near-zero-clamped';
  return 'ok';
}

function mapScoreValidity(value: number): NetcodeHypersphereScoreValidity {
  if (value === 1) return 'invalid-radius';
  return 'valid';
}

function mapSpectralState(value: number): NetcodeHypersphereSpectralState {
  if (value === 1) return 'radius-floor-applied';
  return 'ok';
}

function mapEndiannessPath(value: number):
    NetcodeHypersphereEndiannessDecodePath {
  if (value === 1) return 'byte-swapped';
  return 'little-endian';
}

function mapContractStatus(value: number):
    NetcodeHypersphereKmeansContractStatus {
  if (value === -3001 || value === -2001) return 'unsupported-version';
  if (value === -3002 || value === -2002) return 'invalid-payload';
  if (value === -3003 || value === -2003) return 'nonfinite-value';
  if (value === -3004 || value === -2004) return 'crc-mismatch';
  return 'ok';
}

function computeCrc32(payload: Buffer): number {
  let crc = 0xFFFFFFFF;
  for (const byte of payload) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit += 1) {
      const mask = -(crc & 1);
      crc = (crc >>> 1) ^ (0xEDB88320 & mask);
    }
  }
  return (~crc) >>> 0;
}

export function __decodeHypersphereBufferForTests(outputBuffer: Buffer):
    NetcodeHypersphereOutput {
  if (outputBuffer.length < BANANA_NETCODE_HYPERSPHERE_BUFFER_BYTES) {
    throw new Error(
        'banana_native_v3_netcode_build_hypersphere returned truncated payload');
  }

  const envelopeOffset = BANANA_NETCODE_HYPERSPHERE_PAYLOAD_BYTES;
  const envelopeContractVersion = outputBuffer.readInt32LE(envelopeOffset + 0);
  const envelopeByteOrderTag = outputBuffer.readInt32LE(envelopeOffset + 4);
  const envelopePayloadBytes = outputBuffer.readInt32LE(envelopeOffset + 8);
  const envelopePayloadCrc32 = outputBuffer.readInt32LE(envelopeOffset + 12);
  const envelopeContractStatus = outputBuffer.readInt32LE(envelopeOffset + 16);
  const envelopeStatus = mapContractStatus(envelopeContractStatus);

  if (envelopeContractVersion !== 1 ||
      envelopeStatus === 'unsupported-version') {
    throw new Error('Unsupported native hypersphere contract version');
  }
  if (envelopeByteOrderTag !== 0x01020304) {
    throw new Error('Invalid native hypersphere payload header');
  }
  if (envelopePayloadBytes > BANANA_NETCODE_HYPERSPHERE_PAYLOAD_BYTES) {
    throw new Error(
        'banana_native_v3_netcode_build_hypersphere returned truncated payload');
  }
  if (envelopePayloadBytes <= 0) {
    throw new Error('Invalid native hypersphere payload length');
  }
  if (envelopeStatus === 'invalid-payload') {
    throw new Error('Invalid native hypersphere payload');
  }

  const expectedCrc = envelopePayloadCrc32 >>> 0;
  const actualCrc =
      computeCrc32(outputBuffer.subarray(0, envelopePayloadBytes));
  if (actualCrc !== expectedCrc || envelopeStatus === 'crc-mismatch') {
    throw new Error('Native hypersphere payload CRC mismatch');
  }
  if (envelopeStatus === 'nonfinite-value') {
    throw new Error('Native hypersphere payload contains non-finite values');
  }

  const nodes: NetcodeHypersphereNode[] = [];
  for (let node = 0; node < 4; node += 1) {
    const base = 4 + node * 24;
    nodes.push({
      x: outputBuffer.readFloatLE(base + 0),
      y: outputBuffer.readFloatLE(base + 4),
      z: outputBuffer.readFloatLE(base + 8),
      coherence: outputBuffer.readInt32LE(base + 12),
      inradius: outputBuffer.readFloatLE(base + 16),
      nearestNeighborDistance: outputBuffer.readFloatLE(base + 20),
    });
  }

  const dimensions = outputBuffer.readInt32LE(0);
  const clusterCount = Math.max(0, Math.min(4, outputBuffer.readInt32LE(108)));
  const vectorCount = Math.max(0, Math.min(4, outputBuffer.readInt32LE(112)));

  const centers: NetcodeHypersphereKmeansCenter[] = [];
  for (let cluster = 0; cluster < clusterCount; cluster += 1) {
    const base = 116 + cluster * 72;
    const centerQ16: number[] = [];
    for (let dim = 0; dim < Math.max(0, Math.min(16, dimensions)); dim += 1) {
      centerQ16.push(outputBuffer.readInt32LE(base + 8 + dim * 4));
    }
    centers.push({
      clusterId: outputBuffer.readInt32LE(base + 0),
      memberCount: outputBuffer.readInt32LE(base + 4),
      centerQ16,
    });
  }

  const radii: NetcodeHypersphereKmeansRadius[] = [];
  for (let cluster = 0; cluster < clusterCount; cluster += 1) {
    const base = 404 + cluster * 16;
    radii.push({
      clusterId: outputBuffer.readInt32LE(base + 0),
      nearestNeighborDistanceQ16: outputBuffer.readInt32LE(base + 4),
      inscribedRadiusQ16: outputBuffer.readInt32LE(base + 8),
      radiusState: mapRadiusState(outputBuffer.readInt32LE(base + 12)),
    });
  }

  const weightedVoronoiScores: NetcodeHypersphereWeightedVoronoiScore[] = [];
  for (let index = 0; index < vectorCount * clusterCount; index += 1) {
    const base = 468 + index * 20;
    weightedVoronoiScores.push({
      vectorId: outputBuffer.readInt32LE(base + 0),
      clusterId: outputBuffer.readInt32LE(base + 4),
      distanceToCenterQ16: outputBuffer.readInt32LE(base + 8),
      weightedScoreQ16: outputBuffer.readInt32LE(base + 12),
      scoreValidity: mapScoreValidity(outputBuffer.readInt32LE(base + 16)),
    });
  }

  const spectralProxy: NetcodeHypersphereSpectralProxy[] = [];
  for (let cluster = 0; cluster < clusterCount; cluster += 1) {
    const base = 788 + cluster * 16;
    spectralProxy.push({
      clusterId: outputBuffer.readInt32LE(base + 0),
      frequencyProxyQ16: outputBuffer.readInt32LE(base + 4),
      amplitudeProxyQ16: outputBuffer.readInt32LE(base + 8),
      spectralState: mapSpectralState(outputBuffer.readInt32LE(base + 12)),
    });
  }

  const observability: NetcodeHypersphereObservability = {
    convergenceStatus: outputBuffer.readInt32LE(852),
    iterationCount: outputBuffer.readInt32LE(856),
    assignmentChangesLastIteration: outputBuffer.readInt32LE(860),
    deterministicHash: outputBuffer.readInt32LE(864),
    endiannessDecodePath: mapEndiannessPath(outputBuffer.readInt32LE(868)),
  };

  return {
    dimensions,
    nodes: nodes as
               [NetcodeHypersphereNode, NetcodeHypersphereNode,
                NetcodeHypersphereNode, NetcodeHypersphereNode],
    alignment: outputBuffer.readInt32LE(100),
    radialStability: outputBuffer.readInt32LE(104),
    clusterCount,
    vectorCount,
    centers,
    radii,
    weightedVoronoiScores,
    spectralProxy,
    observability,
    envelope: {
      contractVersion: envelopeContractVersion,
      byteOrderTag: envelopeByteOrderTag,
      payloadBytes: envelopePayloadBytes,
      payloadCrc32: envelopePayloadCrc32,
      status: envelopeStatus,
    },
  };
}

export interface NativeNetcodeService {
  reset(): Promise<void>;
  recordNodeTap(node: number): Promise<void>;
  recordAction(action: number): Promise<void>;
  getLedger(): Promise<NetcodeLedger>;
  buildLearning(signalInput: NetcodeSignalInput):
      Promise<NetcodeLearningOutput>;
  buildReward(signalInput: NetcodeSignalInput, interactionSignal: number):
      Promise<NetcodeRewardOutput>;
  buildLink(input: NetcodeLinkInput): Promise<NetcodeLinkOutput>;
  buildVector(input: NetcodeVectorInput): Promise<NetcodeVectorOutput>;
  buildHypersphere(input: NetcodeVectorInput):
      Promise<NetcodeHypersphereOutput>;
}

type NativeNetcodeSymbols = {
  banana_native_v3_netcode_reset: () => void;
  banana_native_v3_netcode_record_node_tap: (node: number) => void;
  banana_native_v3_netcode_record_action: (action: number) => void;
  banana_native_v3_netcode_get_ledger: (outLedgerPtr: Pointer) => number;
  banana_native_v3_netcode_build_learning:
      (signalInputPtr: Pointer, outOutputPtr: Pointer) => number;
  banana_native_v3_netcode_build_reward:
      (signalInputPtr: Pointer, interactionSignal: number,
       outOutputPtr: Pointer) => number;
  banana_native_v3_netcode_build_link:
      (signalInputPtr: Pointer, outOutputPtr: Pointer) => number;
  banana_native_v3_netcode_build_vector:
      (signalInputPtr: Pointer, outOutputPtr: Pointer) => number;
  banana_native_v3_netcode_build_hypersphere:
      (signalInputPtr: Pointer, outOutputPtr: Pointer) => number;
};

function createNativeBinding(): NativeNetcodeSymbols {
  return loadBananaNativeSymbols<NativeNetcodeSymbols>(
      {
        banana_native_v3_netcode_reset: {
          args: [],
          returns: FFIType.void,
        },
        banana_native_v3_netcode_record_node_tap: {
          args: [FFIType.i32],
          returns: FFIType.void,
        },
        banana_native_v3_netcode_record_action: {
          args: [FFIType.i32],
          returns: FFIType.void,
        },
        banana_native_v3_netcode_get_ledger: {
          args: [FFIType.ptr],
          returns: FFIType.i32,
        },
        banana_native_v3_netcode_build_learning: {
          args: [FFIType.ptr, FFIType.ptr],
          returns: FFIType.i32,
        },
        banana_native_v3_netcode_build_reward: {
          args: [FFIType.ptr, FFIType.i32, FFIType.ptr],
          returns: FFIType.i32,
        },
        banana_native_v3_netcode_build_link: {
          args: [FFIType.ptr, FFIType.ptr],
          returns: FFIType.i32,
        },
        banana_native_v3_netcode_build_vector: {
          args: [FFIType.ptr, FFIType.ptr],
          returns: FFIType.i32,
        },
        banana_native_v3_netcode_build_hypersphere: {
          args: [FFIType.ptr, FFIType.ptr],
          returns: FFIType.i32,
        },
      },
      'netcode FFI',
      {
        fallbackDirs: [
          'out/native/bin',
          'out/v3-native/Debug',
          'out/v3-native/Release',
          'build/native/bin',
          'build/native',
        ],
      },
  );
}

export class NativeFFINetcodeService implements NativeNetcodeService {
  private readonly symbols = createNativeBinding();

  public async reset(): Promise<void> {
    this.symbols.banana_native_v3_netcode_reset();
  }

  public async recordNodeTap(node: number): Promise<void> {
    this.symbols.banana_native_v3_netcode_record_node_tap(node);
  }

  public async recordAction(action: number): Promise<void> {
    this.symbols.banana_native_v3_netcode_record_action(action);
  }

  public async getLedger(): Promise<NetcodeLedger> {
    const ledgerBuffer = Buffer.alloc(20);
    const rc =
        this.symbols.banana_native_v3_netcode_get_ledger(ptr(ledgerBuffer));
    if (rc !== 0) {
      throw new Error(
          `banana_native_v3_netcode_get_ledger failed with status ${rc}`);
    }

    return {
      snapshots: ledgerBuffer.readInt32LE(0),
      inspections: ledgerBuffer.readInt32LE(4),
      trainings: ledgerBuffer.readInt32LE(8),
      routes: ledgerBuffer.readInt32LE(12),
      nodeTaps: ledgerBuffer.readInt32LE(16),
    };
  }

  public async buildLearning(signalInput: NetcodeSignalInput):
      Promise<NetcodeLearningOutput> {
    const signalBuffer = Buffer.alloc(20);
    signalBuffer.writeInt32LE(signalInput.callDensity, 0);
    signalBuffer.writeInt32LE(signalInput.questPercent, 4);
    signalBuffer.writeInt32LE(signalInput.comboStreak, 8);
    signalBuffer.writeInt32LE(signalInput.branchPressure, 12);
    signalBuffer.writeInt32LE(signalInput.workflowDepth, 16);

    const outputBuffer = Buffer.alloc(52);
    const rc = this.symbols.banana_native_v3_netcode_build_learning(
        ptr(signalBuffer), ptr(outputBuffer));
    if (rc !== 0) {
      throw new Error(
          `banana_native_v3_netcode_build_learning failed with status ${rc}`);
    }

    return {
      modelConfidence: outputBuffer.readInt32LE(0),
      trainingAccuracy: outputBuffer.readInt32LE(4),
      policyMomentum: outputBuffer.readInt32LE(8),
      nodeWeights: [
        outputBuffer.readInt32LE(12),
        outputBuffer.readInt32LE(16),
        outputBuffer.readInt32LE(20),
        outputBuffer.readInt32LE(24),
      ],
      recommendedNode: outputBuffer.readInt32LE(28),
      recommendedAction: outputBuffer.readInt32LE(32),
      xpByAction: [
        outputBuffer.readInt32LE(36),
        outputBuffer.readInt32LE(40),
        outputBuffer.readInt32LE(44),
        outputBuffer.readInt32LE(48),
      ],
    };
  }

  public async buildReward(
      signalInput: NetcodeSignalInput,
      interactionSignal: number): Promise<NetcodeRewardOutput> {
    const signalBuffer = Buffer.alloc(20);
    signalBuffer.writeInt32LE(signalInput.callDensity, 0);
    signalBuffer.writeInt32LE(signalInput.questPercent, 4);
    signalBuffer.writeInt32LE(signalInput.comboStreak, 8);
    signalBuffer.writeInt32LE(signalInput.branchPressure, 12);
    signalBuffer.writeInt32LE(signalInput.workflowDepth, 16);

    const outputBuffer = Buffer.alloc(12);
    const rc = this.symbols.banana_native_v3_netcode_build_reward(
        ptr(signalBuffer), interactionSignal, ptr(outputBuffer));
    if (rc !== 0) {
      throw new Error(
          `banana_native_v3_netcode_build_reward failed with status ${rc}`);
    }

    return {
      neuralRelevanceScore: outputBuffer.readInt32LE(0),
      projectedRewardXp: outputBuffer.readInt32LE(4),
      rewardTier: outputBuffer.readInt32LE(8),
    };
  }

  public async buildLink(input: NetcodeLinkInput): Promise<NetcodeLinkOutput> {
    const signalBuffer = Buffer.alloc(28);
    signalBuffer.writeInt32LE(input.callDensity, 0);
    signalBuffer.writeInt32LE(input.questPercent, 4);
    signalBuffer.writeInt32LE(input.playerLevel, 8);
    signalBuffer.writeInt32LE(input.comboStreak, 12);
    signalBuffer.writeInt32LE(input.branchPressure, 16);
    signalBuffer.writeInt32LE(input.dependencyPulse, 20);
    signalBuffer.writeInt32LE(input.interactionSignal, 24);

    const outputBuffer = Buffer.alloc(16);
    const rc = this.symbols.banana_native_v3_netcode_build_link(
        ptr(signalBuffer), ptr(outputBuffer));
    if (rc !== 0) {
      throw new Error(
          `banana_native_v3_netcode_build_link failed with status ${rc}`);
    }

    return {
      intel: outputBuffer.readInt32LE(0),
      objectives: outputBuffer.readInt32LE(4),
      player: outputBuffer.readInt32LE(8),
      ops: outputBuffer.readInt32LE(12),
    };
  }

  public async buildVector(input: NetcodeVectorInput):
      Promise<NetcodeVectorOutput> {
    const signalBuffer = Buffer.alloc(44);
    signalBuffer.writeInt32LE(input.callDensity, 0);
    signalBuffer.writeInt32LE(input.questPercent, 4);
    signalBuffer.writeInt32LE(input.playerLevel, 8);
    signalBuffer.writeInt32LE(input.comboStreak, 12);
    signalBuffer.writeInt32LE(input.branchPressure, 16);
    signalBuffer.writeInt32LE(input.dependencyPulse, 20);
    signalBuffer.writeInt32LE(input.workflowDepth, 24);
    signalBuffer.writeInt32LE(input.neuralRelevanceScore, 28);
    signalBuffer.writeInt32LE(input.networkDimensions, 32);
    signalBuffer.writeInt32LE(input.modelConfidence, 36);
    signalBuffer.writeInt32LE(input.policyMomentum, 40);

    const outputBuffer = Buffer.alloc(276);
    const rc = this.symbols.banana_native_v3_netcode_build_vector(
        ptr(signalBuffer), ptr(outputBuffer));
    if (rc !== 0) {
      throw new Error(
          `banana_native_v3_netcode_build_vector failed with status ${rc}`);
    }

    const dimensions = outputBuffer.readInt32LE(0);
    const boundedDimensions = Math.max(0, Math.min(16, dimensions));
    const nodeVectors: number[][] = [];
    for (let node = 0; node < 4; node += 1) {
      const row: number[] = [];
      for (let dim = 0; dim < boundedDimensions; dim += 1) {
        const offset = 4 + (node * 16 + dim) * 4;
        row.push(outputBuffer.readFloatLE(offset));
      }
      nodeVectors.push(row);
    }

    return {
      dimensions: boundedDimensions,
      nodeVectors,
      contractStrength: [
        outputBuffer.readInt32LE(260),
        outputBuffer.readInt32LE(264),
        outputBuffer.readInt32LE(268),
        outputBuffer.readInt32LE(272),
      ],
    };
  }

  public async buildHypersphere(input: NetcodeVectorInput):
      Promise<NetcodeHypersphereOutput> {
    const signalBuffer = Buffer.alloc(44);
    signalBuffer.writeInt32LE(input.callDensity, 0);
    signalBuffer.writeInt32LE(input.questPercent, 4);
    signalBuffer.writeInt32LE(input.playerLevel, 8);
    signalBuffer.writeInt32LE(input.comboStreak, 12);
    signalBuffer.writeInt32LE(input.branchPressure, 16);
    signalBuffer.writeInt32LE(input.dependencyPulse, 20);
    signalBuffer.writeInt32LE(input.workflowDepth, 24);
    signalBuffer.writeInt32LE(input.neuralRelevanceScore, 28);
    signalBuffer.writeInt32LE(input.networkDimensions, 32);
    signalBuffer.writeInt32LE(input.modelConfidence, 36);
    signalBuffer.writeInt32LE(input.policyMomentum, 40);

    const outputBuffer = Buffer.alloc(BANANA_NETCODE_HYPERSPHERE_BUFFER_BYTES);
    const rc = this.symbols.banana_native_v3_netcode_build_hypersphere(
        ptr(signalBuffer), ptr(outputBuffer));
    if (rc !== 0) {
      throw new Error(
          `banana_native_v3_netcode_build_hypersphere failed with status ${
              rc}`);
    }

    return __decodeHypersphereBufferForTests(outputBuffer);
  }
}

let cachedService: NativeNetcodeService|null = null;

function resolveNetcodeAdapterMode(): 'ffi' {
  const adapterMode =
      (process.env.BANANA_NETCODE_ADAPTER ?? 'ffi').toLowerCase();
  if (adapterMode !== 'ffi' && adapterMode !== 'ffi-only') {
    throw new Error(
        `Unsupported BANANA_NETCODE_ADAPTER mode "${adapterMode}". ` +
        'Use "ffi" to enforce native netcode bindings.');
  }
  return 'ffi';
}

export function __resetNativeNetcodeServiceForTests(): void {
  cachedService = null;
}

export function getNativeNetcodeService(): NativeNetcodeService {
  resolveNetcodeAdapterMode();

  if (!cachedService) {
    cachedService = new NativeFFINetcodeService();
  }

  return cachedService;
}
