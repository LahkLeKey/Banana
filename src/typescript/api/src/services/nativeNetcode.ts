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

export type NetcodeHypersphereNode = {
  readonly x: number; readonly y: number; readonly z: number; readonly coherence:
                                                                           number;
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
};

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

    const outputBuffer = Buffer.alloc(76);
    const rc = this.symbols.banana_native_v3_netcode_build_hypersphere(
        ptr(signalBuffer), ptr(outputBuffer));
    if (rc !== 0) {
      throw new Error(
          `banana_native_v3_netcode_build_hypersphere failed with status ${
              rc}`);
    }

    const nodes: NetcodeHypersphereNode[] = [];
    for (let node = 0; node < 4; node += 1) {
      const base = 4 + node * 16;
      nodes.push({
        x: outputBuffer.readFloatLE(base + 0),
        y: outputBuffer.readFloatLE(base + 4),
        z: outputBuffer.readFloatLE(base + 8),
        coherence: outputBuffer.readInt32LE(base + 12),
      });
    }

    return {
      dimensions: outputBuffer.readInt32LE(0),
      nodes: nodes as
                 [NetcodeHypersphereNode, NetcodeHypersphereNode,
                  NetcodeHypersphereNode, NetcodeHypersphereNode],
      alignment: outputBuffer.readInt32LE(68),
      radialStability: outputBuffer.readInt32LE(72),
    };
  }
}

let cachedService: NativeNetcodeService|null = null;

function resolveNetcodeAdapterMode(): 'ffi' {
  const adapterMode = (process.env.BANANA_NETCODE_ADAPTER ?? 'ffi').toLowerCase();
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
