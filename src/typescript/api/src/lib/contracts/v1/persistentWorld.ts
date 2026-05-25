import {z} from 'zod';

const UuidSchema = z.string().uuid();
const LaneIdSchema = z.string().min(1);
const IdentityHashSchema = z.string().min(16);
const AreaIdSchema = z.string().min(1);
const VersionSchema = z.number().int().nonnegative();

export const AreaIdentitySchema = z.object({
  worldId: UuidSchema,
  laneId: LaneIdSchema,
  chunkX: z.number().int(),
  chunkY: z.number().int(),
  partitionEpoch: z.number().int().nonnegative(),
  identityHash: IdentityHashSchema,
  areaId: AreaIdSchema,
});

export const GenerationFingerprintSchema = z.object({
  derivationPolicyVersion: z.string().min(1),
  normalizationSchemaVersion: z.string().min(1),
  generationInputFingerprint: z.string().min(16),
});

export const AreaStateVersionSchema = z.object({
  areaId: AreaIdSchema,
  areaStateVersion: VersionSchema,
  appliedDeltaThroughSequence: VersionSchema,
  canonicalStateSignature: z.string().min(1),
});

export const RevisitBaselineRequestSchema = z.object({
  worldId: UuidSchema,
  laneId: LaneIdSchema,
  chunkX: z.number().int(),
  chunkY: z.number().int(),
  partitionEpoch: z.number().int().nonnegative(),
});

export const RevisitBaselineViewSchema = z.object({
  areaIdentity: AreaIdentitySchema,
  stateVersion: AreaStateVersionSchema,
  baselineId: z.string().uuid(),
  orchestrationPathId: z.string().min(1),
});

export type AreaIdentity = z.infer<typeof AreaIdentitySchema>;
export type GenerationFingerprint = z.infer<typeof GenerationFingerprintSchema>;
export type AreaStateVersion = z.infer<typeof AreaStateVersionSchema>;
export type RevisitBaselineRequest =
    z.infer<typeof RevisitBaselineRequestSchema>;
export type RevisitBaselineView = z.infer<typeof RevisitBaselineViewSchema>;
