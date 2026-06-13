export type RewardTier = 'Elite Signal'|'Relevant'|'Needs Labeling';

export type ContractNodeId = 'intel'|'objectives'|'player'|'ops';

export type RewardBandPalette = {
  readonly line: string; readonly soft: string; readonly glow: string;
};

export type RewardSignalModel = {
  readonly neuralRelevanceScore: number; readonly projectedRewardXp: number; readonly rewardTier: RewardTier; readonly rewardBand:
                                                                                                                           RewardBandPalette;
};

export type NodeLinkConfidenceModel = {
  readonly intel: number; readonly objectives: number; readonly player: number; readonly ops:
                                                                                             number;
};

export type ContractNodeVectorModel = {
  readonly id: ContractNodeId; readonly dimensions: readonly number[]; readonly contractStrength:
                                                                                    number;
};

export type ContractHypersphereProjectionNode = {
  readonly id: ContractNodeId; readonly x: number; readonly y: number; readonly z: number; readonly coherence: number; readonly inradius: number; readonly nearestNeighborDistance:
                                                                                                                                                               number;
};

export type ContractHypersphereProjectionModel = {
  readonly dimensions: number; readonly nodes: readonly ContractHypersphereProjectionNode[]; readonly alignment: number; readonly radialStability:
                                                                                                                                      number;
};

export type CFileContractNodeModel = {
  readonly filePath: string; readonly signal: number; readonly coupling: number; readonly layerHint:
                                                                                              number;
};

export type NodeInteractionAction = 'snapshot'|'inspect'|'train'|'route';

export type NodeInteractionLedger = {
  readonly snapshots: number; readonly inspections: number; readonly trainings: number; readonly routes: number; readonly nodeTaps:
                                                                                                                              number;
};

export type NodeInteractionLearningModel =
    {
      readonly modelConfidence: number; readonly trainingAccuracy: number; readonly policyMomentum: number; readonly nodeWeights: Readonly<Record<ContractNodeId, number>>; readonly recommendedNode: ContractNodeId; readonly recommendedAction: NodeInteractionAction; readonly xpByAction:
                                                                                                                                                                                                                                                                                      Readonly<
                                                                                                                                                                                                                                                                                          Record<
                                                                                                                                                                                                                                                                                              NodeInteractionAction,
                                                                                                                                                                                                                                                                                              number>>;
    };

const ELITE_BAND: RewardBandPalette = {
  line: '#22c55e',
  soft: 'rgba(34, 197, 94, 0.28)',
  glow: 'rgba(34, 197, 94, 0.42)',
};

const RELEVANT_BAND: RewardBandPalette = {
  line: '#f59e0b',
  soft: 'rgba(245, 158, 11, 0.26)',
  glow: 'rgba(245, 158, 11, 0.4)',
};

const LABELING_BAND: RewardBandPalette = {
  line: '#f97316',
  soft: 'rgba(249, 115, 22, 0.24)',
  glow: 'rgba(249, 115, 22, 0.36)',
};

function clampPercent(value: number): number {
  return Math.min(100, Math.max(0, Math.round(value)));
}

function clampUnit(value: number): number {
  return Math.min(1, Math.max(0, value));
}

function normalizeVector(vector: readonly number[]): number[] {
  const magnitude =
      Math.sqrt(vector.reduce((sum, value) => sum + value * value, 0));
  if (magnitude <= 0.000001) {
    return vector.map(() => 0);
  }
  return vector.map((value) => value / magnitude);
}

function dot(a: readonly number[], b: readonly number[]): number {
  const maxLength = Math.max(a.length, b.length);
  let result = 0;
  for (let index = 0; index < maxLength; index += 1) {
    result += (a[index] ?? 0) * (b[index] ?? 0);
  }
  return result;
}

function euclideanDistance(a: readonly number[], b: readonly number[]): number {
  const maxLength = Math.max(a.length, b.length);
  let sum = 0;
  for (let index = 0; index < maxLength; index += 1) {
    const delta = (a[index] ?? 0) - (b[index] ?? 0);
    sum += delta * delta;
  }
  return Math.sqrt(sum);
}

function projectNormalizedVector(normalizedVector: readonly number[]):
    {x: number; y: number; z: number;} {
  const dimensionCount = Math.max(1, normalizedVector.length);
  let x = 0;
  let y = 0;

  for (let index = 0; index < dimensionCount; index += 1) {
    const angle = (Math.PI * 2 * index) / dimensionCount;
    const value = normalizedVector[index] ?? 0;
    x += value * Math.cos(angle);
    y += value * Math.sin(angle);
  }

  const scale = Math.max(1, Math.sqrt(x * x + y * y));
  const nx = x / scale;
  const ny = y / scale;
  const planarEnergy = Math.min(1, Math.sqrt(nx * nx + ny * ny));
  const z = Math.sqrt(Math.max(0, 1 - planarEnergy * planarEnergy));

  return {x: nx, y: ny, z};
}

function hashString(input: string): number {
  let hash = 0;
  for (let index = 0; index < input.length; index += 1) {
    hash = (hash << 5) - hash + input.charCodeAt(index);
    hash |= 0;
  }
  return Math.abs(hash);
}

export function buildRewardSignalModel(input: {
  readonly callDensity: number; readonly questPercent: number; readonly comboStreak: number; readonly branchPressure: number; readonly workflowDepth:
                                                                                                                                           1 |
      2 | 3;
  readonly interactionSignal?: number;
}): RewardSignalModel {
  const comboSignal = Math.min(100, Math.max(1, input.comboStreak) * 18);
  const stabilitySignal = Math.max(0, 100 - input.branchPressure);
  const interactionSignal = clampPercent(input.interactionSignal ?? 0);
  const neuralRelevanceScore = clampPercent(
      input.callDensity * 0.36 + input.questPercent * 0.26 + comboSignal * 0.2 +
          stabilitySignal * 0.14 + interactionSignal * 0.04,
  );
  const projectedRewardXp = Math.max(
      5, Math.round(neuralRelevanceScore / 4) + input.workflowDepth * 3);

  if (neuralRelevanceScore >= 80) {
    return {
      neuralRelevanceScore,
      projectedRewardXp,
      rewardTier: 'Elite Signal',
      rewardBand: ELITE_BAND,
    };
  }
  if (neuralRelevanceScore >= 55) {
    return {
      neuralRelevanceScore,
      projectedRewardXp,
      rewardTier: 'Relevant',
      rewardBand: RELEVANT_BAND,
    };
  }
  return {
    neuralRelevanceScore,
    projectedRewardXp,
    rewardTier: 'Needs Labeling',
    rewardBand: LABELING_BAND,
  };
}

export function buildNodeLinkConfidenceModel(input: {
  readonly callDensity: number; readonly questPercent: number; readonly playerLevel: number; readonly comboStreak: number; readonly branchPressure: number; readonly dependencyPulse:
                                                                                                                                                                         number;
  readonly interactionSignal?: number;
}): NodeLinkConfidenceModel {
  const interactionSignal = clampPercent(input.interactionSignal ?? 0);
  return {
    intel: clampPercent(input.callDensity * 1.02 + interactionSignal * 0.08),
    objectives:
        clampPercent(input.questPercent * 1.04 + interactionSignal * 0.1),
    player: clampPercent(
        input.playerLevel * 14 + Math.max(1, input.comboStreak) * 8 +
        interactionSignal * 0.06),
    ops: clampPercent(
        (input.branchPressure + input.dependencyPulse) / 2 +
        interactionSignal * 0.1),
  };
}

export function buildContractNodeVectorModel(input: {
  readonly callDensity: number; readonly questPercent: number; readonly playerLevel: number; readonly comboStreak: number; readonly branchPressure: number; readonly dependencyPulse: number; readonly workflowDepth: 1 | 2 | 3; readonly neuralRelevanceScore:
                                                                                                                                                                                                                                              number;
  readonly networkDimensions?: number;
  readonly modelConfidence?: number;
  readonly policyMomentum?: number;
}): readonly ContractNodeVectorModel[] {
  const density = clampUnit(input.callDensity / 100);
  const quest = clampUnit(input.questPercent / 100);
  const player = clampUnit(
      (input.playerLevel * 15 + Math.max(1, input.comboStreak) * 8) / 100);
  const pressure = clampUnit(input.branchPressure / 100);
  const pulse = clampUnit(input.dependencyPulse / 100);
  const depth = clampUnit(input.workflowDepth / 3);
  const relevance = clampUnit(input.neuralRelevanceScore / 100);
  const stability = clampUnit(1 - pressure);
  const confidence = clampUnit((input.modelConfidence ?? 0) / 100);
  const momentum = clampUnit((input.policyMomentum ?? 0) / 100);

  const dimensions = Math.max(6, input.networkDimensions ?? 6);
  const harmonicSource = [
    density,
    quest,
    player,
    pressure,
    pulse,
    depth,
    relevance,
    stability,
    confidence,
    momentum,
  ];
  const expandVector = (baseVector: readonly number[]): number[] => {
    const expanded = [...baseVector];
    while (expanded.length < dimensions) {
      const index = expanded.length;
      const a = harmonicSource[index % harmonicSource.length] ?? 0;
      const b = harmonicSource[(index + 3) % harmonicSource.length] ?? 0;
      const c = harmonicSource[(index + 5) % harmonicSource.length] ?? 0;
      expanded.push(clampUnit(a * 0.52 + b * 0.32 + c * 0.16));
    }
    return expanded;
  };
  const makeStrength = (vector: readonly number[]): number => {
    const avg = vector.reduce((sum, value) => sum + value, 0) / vector.length;
    return clampPercent(avg * 100);
  };

  const intelVector = expandVector([
    density,
    relevance,
    stability,
    (density + pulse) / 2,
    depth * 0.72,
    1 - quest * 0.4,
    confidence,
  ]);
  const objectivesVector = expandVector([
    quest,
    relevance,
    stability,
    depth,
    (quest + density) / 2,
    1 - pressure * 0.5,
    momentum,
  ]);
  const playerVector = expandVector([
    player,
    relevance,
    (player + stability) / 2,
    depth * 0.58,
    Math.max(0.2, pulse),
    Math.max(0.2, density),
    confidence,
  ]);
  const opsVector = expandVector([
    pulse,
    pressure,
    depth,
    (density + quest) / 2,
    relevance,
    Math.max(0.2, 1 - stability * 0.7),
    momentum,
  ]);

  return [
    {
      id: 'intel',
      dimensions: intelVector,
      contractStrength: makeStrength(intelVector)
    },
    {
      id: 'objectives',
      dimensions: objectivesVector,
      contractStrength: makeStrength(objectivesVector)
    },
    {
      id: 'player',
      dimensions: playerVector,
      contractStrength: makeStrength(playerVector)
    },
    {
      id: 'ops',
      dimensions: opsVector,
      contractStrength: makeStrength(opsVector)
    },
  ];
}

export function buildContractHypersphereProjectionModel(
    input: {readonly nodes: readonly ContractNodeVectorModel[];}):
    ContractHypersphereProjectionModel {
  if (input.nodes.length === 0) {
    return {
      dimensions: 0,
      nodes: [],
      alignment: 0,
      radialStability: 0,
    };
  }

  const dimensions = input.nodes[0]?.dimensions.length ?? 0;
  const normalizedNodes =
      input.nodes.map((node) => ({
                        id: node.id,
                        vector: normalizeVector(node.dimensions),
                      }));

  const centroidRaw = new Array(dimensions).fill(0).map((_, index) => {
    return normalizedNodes.reduce(
               (sum, node) => sum + (node.vector[index] ?? 0), 0) /
        normalizedNodes.length;
  });
  const centroid = normalizeVector(centroidRaw);

  let totalCoherence = 0;
  let totalRadius = 0;

  const nodes = normalizedNodes.map((node) => {
    const coherence =
        clampPercent(((dot(node.vector, centroid) + 1) / 2) * 100);
    const projection = projectNormalizedVector(node.vector);
    const radius =
        Math.sqrt(projection.x * projection.x + projection.y * projection.y);
    totalCoherence += coherence;
    totalRadius += radius;

    return {
      id: node.id,
      x: projection.x,
      y: projection.y,
      z: projection.z,
      coherence,
      inradius: 0,
      nearestNeighborDistance: 0,
    };
  });

  for (let index = 0; index < nodes.length; index += 1) {
    let nearestDistance = Number.POSITIVE_INFINITY;
    for (let neighbor = 0; neighbor < normalizedNodes.length; neighbor += 1) {
      if (neighbor === index) {
        continue;
      }
      const candidate = euclideanDistance(
          normalizedNodes[index]?.vector ?? [],
          normalizedNodes[neighbor]?.vector ?? [],
      );
      if (candidate < nearestDistance) {
        nearestDistance = candidate;
      }
    }

    const safeDistance = Number.isFinite(nearestDistance) ? nearestDistance : 0;
    nodes[index] = {
      ...nodes[index],
      nearestNeighborDistance: safeDistance,
      inradius: safeDistance * 0.5,
    };
  }

  const averageRadius = totalRadius / nodes.length;
  const radiusVariance = nodes.reduce((sum, node) => {
    const radius = Math.sqrt(node.x * node.x + node.y * node.y);
    const delta = radius - averageRadius;
    return sum + delta * delta;
  }, 0) / nodes.length;

  return {
    dimensions,
    nodes,
    alignment: clampPercent(totalCoherence / nodes.length),
    radialStability: clampPercent((1 - Math.sqrt(radiusVariance)) * 100),
  };
}

export function buildCFileContractNodeModel(input: {
  readonly files: readonly string[]; readonly contentByFile: ReadonlyMap<string, string>; readonly orbitLayerCount:
                                                                                                       number;
}): readonly CFileContractNodeModel[] {
  const orbitLayerCount = Math.max(2, input.orbitLayerCount);

  return input.files.filter((filePath) => /\.c$/i.test(filePath))
      .map((filePath) => {
        const content = input.contentByFile.get(filePath) ?? '';
        const lineCount = Math.max(1, content.split('\n').length);
        const functionMatches =
            content.match(
                /\b[a-zA-Z_][\w\s\*]*\s+[a-zA-Z_][\w]*\s*\([^)]*\)\s*\{/g) ??
            [];
        const includeMatches = content.match(/^\s*#\s*include\b/gm) ?? [];
        const coupling = clampPercent(Math.min(
            100, includeMatches.length * 8 + functionMatches.length * 6));
        const signal = clampPercent(Math.min(
            100, lineCount / 12 + functionMatches.length * 3 + coupling * 0.4));
        const layerHint = hashString(filePath) % orbitLayerCount;

        return {
          filePath,
          signal,
          coupling,
          layerHint,
        };
      });
}
