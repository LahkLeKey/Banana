export type HealthResponse = {
  status: string;
};

export type BananaResponse = {
  purchases: number; multiplier: number; banana: number; message: string;
};

export type BananaRequest = {
  purchases: number; multiplier: number;
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
