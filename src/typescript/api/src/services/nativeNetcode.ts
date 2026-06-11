import {dlopen, FFIType, type Pointer, ptr, suffix} from 'bun:ffi';
import path from 'node:path';

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

export interface NativeNetcodeService {
  reset(): Promise<void>;
  recordNodeTap(node: number): Promise<void>;
  recordAction(action: number): Promise<void>;
  getLedger(): Promise<NetcodeLedger>;
  buildLearning(signalInput: NetcodeSignalInput):
      Promise<NetcodeLearningOutput>;
}

type NativeNetcodeSymbols = {
  banana_native_v3_netcode_reset: () => void;
  banana_native_v3_netcode_record_node_tap: (node: number) => void;
  banana_native_v3_netcode_record_action: (action: number) => void;
  banana_native_v3_netcode_get_ledger: (outLedgerPtr: Pointer) => number;
  banana_native_v3_netcode_build_learning:
      (signalInputPtr: Pointer, outOutputPtr: Pointer) => number;
};

function resolveNativeLibraryCandidates(): string[] {
  const ext = suffix;
  const names = [`libbanana_native.${ext}`, `banana_native.${ext}`];
  const envPath = process.env.BANANA_NATIVE_PATH;
  const candidates: string[] = [];

  if (envPath && envPath.trim().length > 0) {
    const trimmed = envPath.trim();
    if (trimmed.endsWith(`.${ext}`)) {
      candidates.push(trimmed);
    } else {
      for (const name of names) {
        candidates.push(path.join(trimmed, name));
      }
    }
  }

  const repoRoot = path.resolve(process.cwd(), '../../..');
  const fallbackDirs = [
    'out/native/bin',
    'out/v3-native/Debug',
    'out/v3-native/Release',
    'build/native/bin',
    'build/native',
  ];

  for (const name of names) {
    for (const dir of fallbackDirs) {
      candidates.push(path.join(repoRoot, dir, name));
    }
  }

  return Array.from(new Set(candidates));
}

function createNativeBinding(): NativeNetcodeSymbols {
  const candidates = resolveNativeLibraryCandidates();
  let lastError: unknown = null;

  for (const candidate of candidates) {
    try {
      const library = dlopen(candidate, {
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
      });

      return library.symbols as unknown as NativeNetcodeSymbols;
    } catch (error) {
      lastError = error;
    }
  }

  throw new Error(
      `Unable to load Banana native library for netcode FFI. Candidates: ${
          candidates.join(', ')}. Last error: ${String(lastError)}`);
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
}

let cachedService: NativeNetcodeService|null = null;

export function getNativeNetcodeService(): NativeNetcodeService {
  if (!cachedService) {
    cachedService = new NativeFFINetcodeService();
  }

  return cachedService;
}
