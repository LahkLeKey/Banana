export type HealthResponse = {
  status: string;
};

export type BananaResponse = {
  purchases: number; multiplier: number; banana: number; message: string;
};

export type BananaRequest = {
  purchases: number; multiplier: number;
};
