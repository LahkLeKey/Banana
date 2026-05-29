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

export const ContinuityCheckpointStateSchema =
    z.object({
       checkpointId: z.string().min(1),
       checkpointSequence: z.number().int().nonnegative(),
       checkpointContextTag: z.string().min(1),
       checkpointRouteSignatureTag: z.string().min(1).default('route:default'),
       checkpointFusionLaneTag:
           z.string().min(1).default('fusion:lane-default'),
       checkpointSynthesisPassTag:
           z.string().min(1).default('synthesis:pass-default'),
       checkpointReplayPhaseTag:
           z.string().min(1).default('replay:phase-default'),
       checkpointConvergenceLaneTag:
           z.string().min(1).default('convergence:lane-default'),
       checkpointReconciliationLaneTag:
           z.string().min(1).default('reconciliation:lane-default'),
       checkpointObservationLaneTag:
           z.string().min(1).default('observation:lane-default'),
       checkpointAttunementLaneTag:
           z.string().min(1).default('attunement:lane-default'),
       checkpointEchoLaneTag: z.string().min(1).default('echo:lane-default'),
       checkpointResonanceLaneTag:
           z.string().min(1).default('resonance:lane-default'),
       checkpointHorizonLaneTag:
           z.string().min(1).default('horizon:lane-default'),
       checkpointMeridianLaneTag:
           z.string().min(1).default('meridian:lane-default'),
       checkpointAuroraLaneTag:
           z.string().min(1).default('aurora:lane-default'),
       checkpointStellarLaneTag:
           z.string().min(1).default('stellar:lane-default'),
       objectiveCompletionIds: z.array(z.string().min(1)).default([]),
       profileState: z.record(z.unknown()).default({}),
       profileFingerprint: z.string().min(1),
     }).strict();

export const ContinuityPayloadSchema =
    z.object({
       contractVersion: z.literal('v1'),
       worldId: UuidSchema,
       laneId: LaneIdSchema,
       fromVariantId: z.string().min(1),
       toVariantId: z.string().min(1),
       routeKey: z.string().min(1),
       partitionEpoch: z.number().int().nonnegative(),
       chunkX: z.number().int(),
       chunkY: z.number().int(),
       checkpoint: ContinuityCheckpointStateSchema,
       transitionSignature: z.string().regex(/^[a-f0-9]{32,128}$/i),
     }).strict();

export const ContinuityCheckpointCommitRequestSchema =
    z.object({
       continuityPayload: ContinuityPayloadSchema,
       baseAreaStateVersion: VersionSchema,
       idempotencyKey: z.string().min(8),
     }).strict();

export const ContinuityCheckpointCommitViewSchema = z.object({
  areaIdentity: AreaIdentitySchema,
  baselineId: z.string().uuid(),
  areaStateVersion: AreaStateVersionSchema,
  continuityPayload: ContinuityPayloadSchema,
});

export type AreaIdentity = z.infer<typeof AreaIdentitySchema>;
export type GenerationFingerprint = z.infer<typeof GenerationFingerprintSchema>;
export type AreaStateVersion = z.infer<typeof AreaStateVersionSchema>;
export type RevisitBaselineRequest =
    z.infer<typeof RevisitBaselineRequestSchema>;
export type RevisitBaselineView = z.infer<typeof RevisitBaselineViewSchema>;
export type ContinuityCheckpointState =
    z.infer<typeof ContinuityCheckpointStateSchema>;
export type ContinuityPayload = z.infer<typeof ContinuityPayloadSchema>;
export type ContinuityCheckpointCommitRequest =
    z.infer<typeof ContinuityCheckpointCommitRequestSchema>;
export type ContinuityCheckpointCommitView =
    z.infer<typeof ContinuityCheckpointCommitViewSchema>;
