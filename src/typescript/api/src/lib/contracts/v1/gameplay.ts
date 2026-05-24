import {z} from 'zod';

const PlayerId = z.string().min(1);
const SessionId = z.string().min(1);
const IdempotencyKey = z.string().min(8);

export const GameplayAdmissionRequestSchema = z.object({
  playerId: PlayerId,
  queueIntent: z.enum(['ranked', 'casual', 'private', 'practice']),
  idempotencyKey: IdempotencyKey,
});

export const GameplayAdmissionResponseSchema = z.object({
  sessionId: SessionId,
  admissionStatus: z.enum(['admitted', 'already-admitted']),
  continuityWindowExpiresAt: z.string(),
});

export const GameplayCommandRequestSchema = z.object({
  playerId: PlayerId,
  commandType: z.string().min(1),
  commandPayload: z.record(z.unknown()),
  commandSequence: z.number().int().nonnegative(),
  idempotencyKey: IdempotencyKey,
});

export const GameplayCommandResponseSchema = z.object({
  authoritativeTick: z.number().int().nonnegative(),
  resolutionCode: z.string(),
  conflictHandled: z.boolean(),
});

export const GameplayRejoinRequestSchema = z.object({
  playerId: PlayerId,
  continuityToken: z.string().min(1),
});

export const GameplayRejoinResponseSchema = z.object({
  rejoinStatus: z.enum(['restored', 'expired', 'not-found']),
  restoredSnapshotRef: z.string(),
});

export const GameplayTerminateRequestSchema = z.object({
  playerId: PlayerId,
  terminationReason: z.string().min(1),
  idempotencyKey: IdempotencyKey,
});

export const GameplayTerminateResponseSchema = z.object({
  terminationStatus: z.enum(['terminated', 'already-terminated']),
  outcomeRef: z.string(),
});

export type GameplayAdmissionRequest =
    z.infer<typeof GameplayAdmissionRequestSchema>;
export type GameplayAdmissionResponse =
    z.infer<typeof GameplayAdmissionResponseSchema>;
export type GameplayCommandRequest =
    z.infer<typeof GameplayCommandRequestSchema>;
export type GameplayCommandResponse =
    z.infer<typeof GameplayCommandResponseSchema>;
export type GameplayRejoinRequest = z.infer<typeof GameplayRejoinRequestSchema>;
export type GameplayRejoinResponse =
    z.infer<typeof GameplayRejoinResponseSchema>;
export type GameplayTerminateRequest =
    z.infer<typeof GameplayTerminateRequestSchema>;
export type GameplayTerminateResponse =
    z.infer<typeof GameplayTerminateResponseSchema>;
