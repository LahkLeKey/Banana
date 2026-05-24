import {z} from 'zod';

export const PlayerAccountViewSchema = z.object({
  playerId: z.string().min(1),
  accountStatus: z.enum(['active', 'suspended', 'restricted', 'deleted']),
  profile: z.record(z.unknown()),
  version: z.number().int().nonnegative(),
});

export const PlayerAccountUpdateRequestSchema = z.object({
  profilePatch: z.record(z.unknown()),
  expectedVersion: z.number().int().nonnegative(),
  idempotencyKey: z.string().min(8),
});

export const PlayerProgressionViewSchema = z.object({
  playerId: z.string().min(1),
  progression: z.record(z.unknown()),
  freshnessTimestamp: z.string(),
  snapshotRef: z.string().min(1),
});

export const PlayerInventoryViewSchema = z.object({
  playerId: z.string().min(1),
  inventory: z.array(z.record(z.unknown())),
  freshnessTimestamp: z.string(),
  snapshotRef: z.string().min(1),
});

export const PlayerInsightsViewSchema = z.object({
  playerId: z.string().min(1),
  sessionSummary: z.record(z.unknown()),
  progressionSummary: z.record(z.unknown()),
  inventoryTrendSummary: z.record(z.unknown()),
  noData: z.boolean(),
  freshnessTimestamp: z.string(),
});

export type PlayerAccountView = z.infer<typeof PlayerAccountViewSchema>;
