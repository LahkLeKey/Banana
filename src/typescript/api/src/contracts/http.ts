export type HealthResponse = {
  status: string;
};

export type BananaRequest = {
  purchases: number; multiplier: number;
};

export type BananaResponse = {
  purchases: number; multiplier: number; banana: number; message: string;
};

export type BananaBatchCreateRequest = {
  batchId: string; originFarm: string; storageTempC: number;
  ethyleneExposure: number;
  estimatedShelfLifeDays: number;
};

export type BananaBatchResponse = {
  batchId: string; originFarm: string; exportStatus: string;
  storageTempC: number;
  ethyleneExposure: number;
  estimatedShelfLifeDays: number;
};

export type BananaRipenessRequest = {
  batchId: string; temperatureHistory: number[]; daysSinceHarvest: number;
  ethyleneExposure: number;
  mechanicalDamage: number;
  storageTempC?: number;
};

export type BananaRipenessResponse = {
  batchId: string; predictedStage: string; shelfLifeHours: number;
  ripeningIndex: number;
  spoilageProbability: number;
  coldChainRisk: number;
};

export type BananaMlRegressionRequest = {
  features: number[];
};

export type BananaMlRegressionResponse = {
  score: number;
};

export type BananaMlBinaryRequest = {
  features: number[];
};

export type BananaMlBinaryConfusionMatrix = {
  truePositive: number; falsePositive: number; falseNegative: number;
  trueNegative: number;
};

export type BananaMlBinaryResponse = {
  predictedLabel: string; bananaProbability: number;
  notBananaProbability: number;
  decisionMargin: number;
  jaccardSimilarity: number;
  confusionMatrix: BananaMlBinaryConfusionMatrix;
};

export type BananaMlTransformerRequest = {
  tokenFeatures: number[];
};

export type BananaMlTransformerResponse = {
  predictedLabel: string; bananaProbability: number;
  notBananaProbability: number;
  attentionFocus: number;
};

export type NotBananaPolymorphicActor = {
  actorType: string; actorId: string | null;
};

export type NotBananaPolymorphicEntity = {
  entityType: string; entityId: string | null;
};

export type NotBananaJunkRequest = {
  actors?: unknown[];
  entities?: unknown[];
  actor?: unknown;
  entity?: unknown;
  junk?: unknown;
  metadata?: Record<string, unknown>;
};

export type NotBananaJunkClassification = 'NOT_BANANA'|'MAYBE_BANANA';

export type NotBananaJunkResponse = {
  classification: NotBananaJunkClassification; bananaProbability: number;
  notBananaProbability: number;
  junkConfidence: number;
  actorCount: number;
  entityCount: number;
  normalizedActors: NotBananaPolymorphicActor[];
  normalizedEntities: NotBananaPolymorphicEntity[];
  message: string;
};
