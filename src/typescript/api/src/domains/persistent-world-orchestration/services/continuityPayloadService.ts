import {createHash} from 'node:crypto';

import {type ContinuityPayload, ContinuityPayloadSchema} from '../../../lib/contracts/v1/persistentWorld.ts';
import {validationError} from '../../../lib/errors/domainErrors.ts';

const SUPPORTED_VARIANT_IDS = new Set([
  'neo-musa',
  'metro-crescent',
  'north-crown',
  'stem-territories',
]);

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
  return [normalizeVariantId(fromVariantId), normalizeVariantId(toVariantId)]
      .sort()
      .join('::');
}

function normalizeVariantId(variantId: string): string {
  return variantId.trim().toLowerCase();
}

function isSupportedVariantId(variantId: string): boolean {
  return SUPPORTED_VARIANT_IDS.has(variantId);
}

function normalizeLaneId(laneId: string): string {
  return laneId.trim().toLowerCase();
}

function normalizeWorldId(worldId: string): string {
  return worldId.trim().toLowerCase();
}

function normalizeCheckpointContextTag(contextTag: string): string {
  return contextTag.trim().toLowerCase();
}

function normalizeProfileFingerprint(profileFingerprint: string): string {
  return profileFingerprint.trim().toLowerCase();
}

function normalizeCheckpointId(checkpointId: string): string {
  return checkpointId.trim().toLowerCase();
}

function normalizeCheckpointRouteSignatureTag(tag: string): string {
  return tag.trim().toLowerCase();
}

function normalizeCheckpointFusionLaneTag(tag: string): string {
  return tag.trim().toLowerCase();
}

function normalizeCheckpointSynthesisPassTag(tag: string): string {
  return tag.trim().toLowerCase();
}

function normalizeCheckpointReplayPhaseTag(tag: string): string {
  return tag.trim().toLowerCase();
}

function normalizeCheckpointConvergenceLaneTag(tag: string): string {
  return tag.trim().toLowerCase();
}

function normalizeCheckpointReconciliationLaneTag(tag: string): string {
  return tag.trim().toLowerCase();
}

function normalizeCheckpointObservationLaneTag(tag: string): string {
  return tag.trim().toLowerCase();
}

function normalizeObjectiveCompletionIds(ids: readonly string[]): string[] {
  return Array.from(new Set(ids))
      .sort((left, right) => left.localeCompare(right));
}

function objectiveCompletionIdsEqual(
    left: readonly string[], right: readonly string[]): boolean {
  if (left.length !== right.length) {
    return false;
  }
  for (let index = 0; index < left.length; index += 1) {
    if (left[index] !== right[index]) {
      return false;
    }
  }
  return true;
}

function usesLegacyRouteDelimiter(routeKey: string): boolean {
  return routeKey.includes('->') || routeKey.includes('|') ||
      routeKey.includes('=>');
}

function makeExpectedTransitionSignature(payload: ContinuityPayload): string {
  const checkpointRouteSignatureTag =
      payload.checkpoint.checkpointRouteSignatureTag ?? 'route:default';
  const checkpointFusionLaneTag =
      payload.checkpoint.checkpointFusionLaneTag ?? 'fusion:lane-default';
  const checkpointSynthesisPassTag =
      payload.checkpoint.checkpointSynthesisPassTag ?? 'synthesis:pass-default';
  const checkpointReplayPhaseTag =
      payload.checkpoint.checkpointReplayPhaseTag ?? 'replay:phase-default';
  const checkpointConvergenceLaneTag =
      payload.checkpoint.checkpointConvergenceLaneTag ??
      'convergence:lane-default';
  const checkpointReconciliationLaneTag =
      payload.checkpoint.checkpointReconciliationLaneTag ??
      'reconciliation:lane-default';
  const checkpointObservationLaneTag =
      payload.checkpoint.checkpointObservationLaneTag ??
      'observation:lane-default';
  const checkpointAttunementLaneTag =
      payload.checkpoint.checkpointAttunementLaneTag ??
      'attunement:lane-default';
  const checkpointEchoLaneTag =
      payload.checkpoint.checkpointEchoLaneTag ?? 'echo:lane-default';
  const checkpointResonanceLaneTag =
      payload.checkpoint.checkpointResonanceLaneTag ?? 'resonance:lane-default';
  const checkpointHorizonLaneTag =
      payload.checkpoint.checkpointHorizonLaneTag ?? 'horizon:lane-default';
  const checkpointMeridianLaneTag =
      payload.checkpoint.checkpointMeridianLaneTag ?? 'meridian:lane-default';
  const checkpointAuroraLaneTag =
      payload.checkpoint.checkpointAuroraLaneTag ?? 'aurora:lane-default';
  const checkpointStellarLaneTag =
      payload.checkpoint.checkpointStellarLaneTag ?? 'stellar:lane-default';

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
      checkpointRouteSignatureTag,
      checkpointFusionLaneTag,
      checkpointSynthesisPassTag,
      checkpointReplayPhaseTag,
      checkpointConvergenceLaneTag,
      checkpointReconciliationLaneTag,
      checkpointObservationLaneTag,
      checkpointAttunementLaneTag,
      checkpointEchoLaneTag,
      checkpointResonanceLaneTag,
      checkpointHorizonLaneTag,
      checkpointMeridianLaneTag,
      checkpointAuroraLaneTag,
      checkpointStellarLaneTag,
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
      const normalizedWorldId = normalizeWorldId(payload.worldId);
      const normalizedFromVariantId = normalizeVariantId(payload.fromVariantId);
      const normalizedToVariantId = normalizeVariantId(payload.toVariantId);
      const normalizedLaneId = normalizeLaneId(payload.laneId);
      const normalizedCheckpointContextTag = normalizeCheckpointContextTag(
          payload.checkpoint.checkpointContextTag);

      if (payload.worldId !== normalizedWorldId) {
        throw validationError(
            'persistent_world_continuity_world_identity_mismatch', {
              worldId: payload.worldId,
              expectedWorldId: normalizedWorldId,
              reason: 'non_canonical_world_id_format',
            });
      }

      if (payload.laneId !== normalizedLaneId) {
        throw validationError(
            'persistent_world_continuity_lane_identity_mismatch', {
              laneId: payload.laneId,
              expectedLaneId: normalizedLaneId,
              reason: 'non_canonical_lane_id_format',
            });
      }

      if (payload.checkpoint.checkpointContextTag !==
          normalizedCheckpointContextTag) {
        throw validationError(
            'persistent_world_continuity_checkpoint_context_mismatch', {
              checkpointContextTag: payload.checkpoint.checkpointContextTag,
              expectedCheckpointContextTag: normalizedCheckpointContextTag,
              reason: 'non_canonical_checkpoint_context_format',
            });
      }

      const normalizedProfileFingerprint =
          normalizeProfileFingerprint(payload.checkpoint.profileFingerprint);
      if (payload.checkpoint.profileFingerprint !==
          normalizedProfileFingerprint) {
        throw validationError(
            'persistent_world_continuity_profile_fingerprint_mismatch', {
              profileFingerprint: payload.checkpoint.profileFingerprint,
              expectedProfileFingerprint: normalizedProfileFingerprint,
              reason: 'non_canonical_profile_fingerprint_format',
            });
      }

      const normalizedCheckpointId =
          normalizeCheckpointId(payload.checkpoint.checkpointId);
      if (payload.checkpoint.checkpointId !== normalizedCheckpointId) {
        throw validationError(
            'persistent_world_continuity_checkpoint_id_mismatch', {
              checkpointId: payload.checkpoint.checkpointId,
              expectedCheckpointId: normalizedCheckpointId,
              reason: 'non_canonical_checkpoint_id_format',
            });
      }

      const providedRouteSignatureTag =
          payload.checkpoint.checkpointRouteSignatureTag ?? 'route:default';
      const normalizedRouteSignatureTag =
          normalizeCheckpointRouteSignatureTag(providedRouteSignatureTag);
      if (providedRouteSignatureTag !== normalizedRouteSignatureTag) {
        throw validationError(
            'persistent_world_continuity_checkpoint_route_signature_mismatch', {
              checkpointRouteSignatureTag: providedRouteSignatureTag,
              expectedCheckpointRouteSignatureTag: normalizedRouteSignatureTag,
              reason: 'non_canonical_checkpoint_route_signature_format',
            });
      }

      const providedFusionLaneTag =
          payload.checkpoint.checkpointFusionLaneTag ?? 'fusion:lane-default';
      const normalizedFusionLaneTag =
          normalizeCheckpointFusionLaneTag(providedFusionLaneTag);
      if (providedFusionLaneTag !== normalizedFusionLaneTag) {
        throw validationError(
            'persistent_world_continuity_checkpoint_fusion_lane_mismatch', {
              checkpointFusionLaneTag: providedFusionLaneTag,
              expectedCheckpointFusionLaneTag: normalizedFusionLaneTag,
              reason: 'non_canonical_checkpoint_fusion_lane_format',
            });
      }

      const providedSynthesisPassTag =
          payload.checkpoint.checkpointSynthesisPassTag ??
          'synthesis:pass-default';
      const normalizedSynthesisPassTag =
          normalizeCheckpointSynthesisPassTag(providedSynthesisPassTag);
      if (providedSynthesisPassTag !== normalizedSynthesisPassTag) {
        throw validationError(
            'persistent_world_continuity_checkpoint_synthesis_pass_mismatch', {
              checkpointSynthesisPassTag: providedSynthesisPassTag,
              expectedCheckpointSynthesisPassTag: normalizedSynthesisPassTag,
              reason: 'non_canonical_checkpoint_synthesis_pass_format',
            });
      }

      const providedReplayPhaseTag =
          payload.checkpoint.checkpointReplayPhaseTag ?? 'replay:phase-default';
      const normalizedReplayPhaseTag =
          normalizeCheckpointReplayPhaseTag(providedReplayPhaseTag);
      if (providedReplayPhaseTag !== normalizedReplayPhaseTag) {
        throw validationError(
            'persistent_world_continuity_checkpoint_replay_phase_mismatch', {
              checkpointReplayPhaseTag: providedReplayPhaseTag,
              expectedCheckpointReplayPhaseTag: normalizedReplayPhaseTag,
              reason: 'non_canonical_checkpoint_replay_phase_format',
            });
      }

      const providedConvergenceLaneTag =
          payload.checkpoint.checkpointConvergenceLaneTag ??
          'convergence:lane-default';
      const normalizedConvergenceLaneTag =
          normalizeCheckpointConvergenceLaneTag(providedConvergenceLaneTag);
      if (providedConvergenceLaneTag !== normalizedConvergenceLaneTag) {
        throw validationError(
            'persistent_world_continuity_checkpoint_convergence_lane_mismatch',
            {
              checkpointConvergenceLaneTag: providedConvergenceLaneTag,
              expectedCheckpointConvergenceLaneTag:
                  normalizedConvergenceLaneTag,
              reason: 'non_canonical_checkpoint_convergence_lane_format',
            });
      }

      const providedReconciliationLaneTag =
          payload.checkpoint.checkpointReconciliationLaneTag ??
          'reconciliation:lane-default';
      const normalizedReconciliationLaneTag =
          normalizeCheckpointReconciliationLaneTag(
              providedReconciliationLaneTag);
      if (providedReconciliationLaneTag !== normalizedReconciliationLaneTag) {
        throw validationError(
            'persistent_world_continuity_checkpoint_reconciliation_lane_mismatch',
            {
              checkpointReconciliationLaneTag: providedReconciliationLaneTag,
              expectedCheckpointReconciliationLaneTag:
                  normalizedReconciliationLaneTag,
              reason: 'non_canonical_checkpoint_reconciliation_lane_format',
            });
      }

      const providedObservationLaneTag =
          payload.checkpoint.checkpointObservationLaneTag ??
          'observation:lane-default';
      const normalizedObservationLaneTag =
          normalizeCheckpointObservationLaneTag(providedObservationLaneTag);
      if (providedObservationLaneTag !== normalizedObservationLaneTag) {
        throw validationError(
            'persistent_world_continuity_checkpoint_observation_lane_mismatch',
            {
              checkpointObservationLaneTag: providedObservationLaneTag,
              expectedCheckpointObservationLaneTag:
                  normalizedObservationLaneTag,
              reason: 'non_canonical_checkpoint_observation_lane_format',
            });
      }

      const normalizedObjectiveCompletionIds = normalizeObjectiveCompletionIds(
          payload.checkpoint.objectiveCompletionIds);
      if (!objectiveCompletionIdsEqual(
              payload.checkpoint.objectiveCompletionIds,
              normalizedObjectiveCompletionIds)) {
        throw validationError(
            'persistent_world_continuity_objective_completion_ids_mismatch', {
              objectiveCompletionIds: payload.checkpoint.objectiveCompletionIds,
              expectedObjectiveCompletionIds: normalizedObjectiveCompletionIds,
              reason: 'non_canonical_objective_completion_ids_order',
            });
      }

      if (payload.fromVariantId !== normalizedFromVariantId ||
          payload.toVariantId !== normalizedToVariantId) {
        throw validationError(
            'persistent_world_continuity_variant_identity_mismatch', {
              fromVariantId: payload.fromVariantId,
              toVariantId: payload.toVariantId,
              expectedFromVariantId: normalizedFromVariantId,
              expectedToVariantId: normalizedToVariantId,
              reason: 'non_canonical_variant_id_format',
            });
      }

      if (!isSupportedVariantId(normalizedFromVariantId) ||
          !isSupportedVariantId(normalizedToVariantId)) {
        throw validationError(
            'persistent_world_continuity_variant_unsupported', {
              fromVariantId: payload.fromVariantId,
              toVariantId: payload.toVariantId,
              reason: 'unsupported_variant_id',
            });
      }

      if (normalizedFromVariantId === normalizedToVariantId) {
        throw validationError(
            'persistent_world_continuity_variant_disconnected', {
              fromVariantId: normalizedFromVariantId,
              toVariantId: normalizedToVariantId,
              reason: 'same_variant_transition_not_allowed',
            });
      }

      if (usesLegacyRouteDelimiter(payload.routeKey)) {
        throw validationError(
            'persistent_world_continuity_route_key_legacy_format', {
              providedRouteKey: payload.routeKey,
              reason: 'legacy_route_delimiter_detected',
            });
      }

      const expectedRouteKey =
          toCanonicalRouteKey(normalizedFromVariantId, normalizedToVariantId);
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

      const tag = normalizedCheckpointContextTag;
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
