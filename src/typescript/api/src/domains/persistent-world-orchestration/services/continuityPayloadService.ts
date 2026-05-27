import {createHash} from 'node:crypto';

import {type ContinuityPayload, ContinuityPayloadSchema} from '../../../lib/contracts/v1/persistentWorld.ts';
import {validationError} from '../../../lib/errors/domainErrors.ts';

export interface ContinuityPayloadService {
  validateAndNormalize(raw: unknown): ContinuityPayload;
  canonicalRouteKey(fromVariantId: string, toVariantId: string): string;
  expectedTransitionSignature(payload: ContinuityPayload): string;
}

function toStableJson(value: unknown): string {
  if (Array.isArray(value)) {
    return `[${value.map((item) => toStableJson(item)).join(',')}]`;
  }

  if (!value || typeof value !== 'object') {
    return JSON.stringify(value);
  }

  const entries = Object.entries(value as Record<string, unknown>)
                      .sort(([left], [right]) => left.localeCompare(right));
  return `{${
      entries
          .map(([key,
                 nested]) => `${JSON.stringify(key)}:${toStableJson(nested)}`)
          .join(',')}}`;
}

function hashSha256(payload: string): string {
  return createHash('sha256').update(payload).digest('hex');
}

function toCanonicalRouteKey(
    fromVariantId: string, toVariantId: string): string {
  return [fromVariantId.trim(), toVariantId.trim()].sort().join('::');
}

function makeExpectedTransitionSignature(payload: ContinuityPayload): string {
  return hashSha256(toStableJson({
    contractVersion: payload.contractVersion,
    worldId: payload.worldId,
    laneId: payload.laneId,
    routeKey: payload.routeKey,
    fromVariantId: payload.fromVariantId,
    toVariantId: payload.toVariantId,
    partitionEpoch: payload.partitionEpoch,
    chunkX: payload.chunkX,
    chunkY: payload.chunkY,
    checkpoint: {
      checkpointId: payload.checkpoint.checkpointId,
      checkpointSequence: payload.checkpoint.checkpointSequence,
      checkpointContextTag: payload.checkpoint.checkpointContextTag,
      objectiveCompletionIds: payload.checkpoint.objectiveCompletionIds,
      profileFingerprint: payload.checkpoint.profileFingerprint,
    },
  }));
}

export function createContinuityPayloadService(): ContinuityPayloadService {
  return {
    validateAndNormalize(raw: unknown): ContinuityPayload {
      const parsed = ContinuityPayloadSchema.safeParse(raw);
      if (!parsed.success) {
        throw validationError('persistent_world_continuity_payload_invalid', {
          issues: parsed.error.issues.map((issue) => ({
                                            path: issue.path.join('.'),
                                            code: issue.code,
                                            message: issue.message,
                                          })),
        });
      }

      const payload = parsed.data;

      if (payload.fromVariantId === payload.toVariantId) {
        throw validationError(
            'persistent_world_continuity_variant_disconnected', {
              fromVariantId: payload.fromVariantId,
              toVariantId: payload.toVariantId,
              reason: 'same_variant_transition_not_allowed',
            });
      }

      const expectedRouteKey =
          toCanonicalRouteKey(payload.fromVariantId, payload.toVariantId);
      if (payload.routeKey !== expectedRouteKey) {
        throw validationError(
            'persistent_world_continuity_route_key_mismatch', {
              expectedRouteKey,
              providedRouteKey: payload.routeKey,
              fromVariantId: payload.fromVariantId,
              toVariantId: payload.toVariantId,
            });
      }

      const expectedSignature = makeExpectedTransitionSignature(payload);
      if (payload.transitionSignature !== expectedSignature) {
        throw validationError(
            'persistent_world_continuity_signature_mismatch', {
              expectedTransitionSignature: expectedSignature,
              providedTransitionSignature: payload.transitionSignature,
              routeKey: payload.routeKey,
            });
      }

      const tag = payload.checkpoint.checkpointContextTag.trim().toLowerCase();
      if (tag.startsWith('disabled:') || tag.startsWith('unsupported:')) {
        throw validationError(
            'persistent_world_continuity_checkpoint_context_unsupported', {
              checkpointContextTag: payload.checkpoint.checkpointContextTag,
              reason: tag.startsWith('disabled:') ? 'disabled_context_tag' :
                                                    'unsupported_context_tag',
            });
      }

      return payload;
    },

    canonicalRouteKey(fromVariantId: string, toVariantId: string): string {
      return toCanonicalRouteKey(fromVariantId, toVariantId);
    },

    expectedTransitionSignature(payload: ContinuityPayload): string {
      return makeExpectedTransitionSignature(payload);
    },
  };
}
