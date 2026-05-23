export type BananaEngineModule = {
  ccall:
      (name: string, returnType: string, argTypes: string[], args: unknown[]) =>
          unknown;
  cwrap: (name: string, returnType: string, argTypes: string[]) =>
      (...args: unknown[]) => unknown;
};

export type WorldPosition = {
  x: number; z: number;
};

export type DancingBananaParticle = {
  id: number; leftPct: number; topPct: number; durationMs: number;
  delayMs: number;
  sizeRem: number;
  glyph: string;
};

export const PBJ_PICKUP_WORLD_POSITION: WorldPosition = {
  x: 1.8,
  z: 1.6
};
export const PBJ_PICKUP_COLLECT_RADIUS = 1.75;