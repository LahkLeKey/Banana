import type {ContractNodeId, NodeInteractionAction, NodeInteractionLearningModel, NodeInteractionLedger,} from './network-domain';

export type NativeNetcodeSignalInput = {
  readonly callDensity: number; readonly questPercent: number; readonly comboStreak: number; readonly branchPressure: number; readonly workflowDepth:
                                                                                                                                           1 |
      2 | 3;
};

type NativeNetcodeLearningResult =
    {
      readonly modelConfidence: number; readonly trainingAccuracy: number; readonly policyMomentum: number; readonly nodeWeights: readonly[number, number, number, number]; readonly recommendedNode: number; readonly recommendedAction: number; readonly xpByAction: readonly[number, number, number, number];
    };

type NativeNetcodeBridge = {
  buildLearningModel(
      ledger: NodeInteractionLedger,
      signalInput: NativeNetcodeSignalInput): NativeNetcodeLearningResult;
};

function clampPercent(value: number): number {
  return Math.min(100, Math.max(0, Math.round(value)));
}

function clampXp(value: number): number {
  return Math.max(1, Math.round(value));
}

function resolveNativeBridge(): NativeNetcodeBridge {
  const globalScope = globalThis as typeof globalThis & {
    __BANANA_NATIVE_NETCODE__?: NativeNetcodeBridge;
  };

  if (globalScope.__BANANA_NATIVE_NETCODE__ &&
      typeof globalScope.__BANANA_NATIVE_NETCODE__.buildLearningModel ===
          'function') {
    return globalScope.__BANANA_NATIVE_NETCODE__;
  }

  throw new Error(
      'BANANA native netcode bridge is not configured on globalThis.__BANANA_NATIVE_NETCODE__.');
}

export function tryBuildNativeNodeInteractionLearningModel(
    ledger: NodeInteractionLedger,
    input: NativeNetcodeSignalInput,
    ): NodeInteractionLearningModel {
  const nativeBridge = resolveNativeBridge();
  const result = nativeBridge.buildLearningModel(ledger, input);

  return {
    modelConfidence: clampPercent(result.modelConfidence),
    trainingAccuracy: clampPercent(result.trainingAccuracy),
    policyMomentum: clampPercent(result.policyMomentum),
    nodeWeights: {
      intel: clampPercent(result.nodeWeights[0] ?? 0),
      objectives: clampPercent(result.nodeWeights[1] ?? 0),
      player: clampPercent(result.nodeWeights[2] ?? 0),
      ops: clampPercent(result.nodeWeights[3] ?? 0),
    },
    recommendedNode: mapNativeNodeId(result.recommendedNode),
    recommendedAction: mapNativeActionId(result.recommendedAction),
    xpByAction: {
      snapshot: clampXp(result.xpByAction[0] ?? 0),
      inspect: clampXp(result.xpByAction[1] ?? 0),
      train: clampXp(result.xpByAction[2] ?? 0),
      route: clampXp(result.xpByAction[3] ?? 0),
    },
  };
}

export function mapNativeNodeId(value: number): ContractNodeId {
  if (value === 1) {
    return 'objectives';
  }
  if (value === 2) {
    return 'player';
  }
  if (value === 3) {
    return 'ops';
  }
  return 'intel';
}

export function mapNativeActionId(value: number): NodeInteractionAction {
  if (value === 1) {
    return 'inspect';
  }
  if (value === 2) {
    return 'train';
  }
  if (value === 3) {
    return 'route';
  }
  return 'snapshot';
}
