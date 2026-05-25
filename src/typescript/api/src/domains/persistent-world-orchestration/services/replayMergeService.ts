import {createHash} from 'node:crypto';

import type {AreaIdentity, RevisitBaselineView} from '../../../lib/contracts/v1/persistentWorld.ts';
import {makeDeterministicCanonicalStateSignature} from '../normalization.ts';
import {type AuthoritativeBaselineRecord, type BaselineRepository, createBaselineRepository} from '../persistence/baselineRepository.ts';
import {createDeltaRepository, type DeltaRepository} from '../persistence/deltaRepository.ts';

import {type AreaStateVersionService, createAreaStateVersionService} from './areaStateVersionService.ts';

export type ReplayDeltaRequest = {
  areaIdentity: AreaIdentity; baselineId: string; baseAreaStateVersion: number;
  idempotencyKey: string;
  deltaPayload: Record<string, unknown>;
};

export type ReplayMergeResult = {
  areaIdentity: AreaIdentity; baselineId: string; areaStateVersion: number;
  appliedDeltaThroughSequence: number;
  canonicalStateSignature: string;
};

export interface ReplayMergeService {
  ensureAuthoritativeBaseline(view: RevisitBaselineView):
      AuthoritativeBaselineRecord;
  applyDelta(request: ReplayDeltaRequest): ReplayMergeResult;
  replay(areaId: string): ReplayMergeResult|null;
}

function stableJson(value: unknown): string {
  if (Array.isArray(value)) {
    return `[${value.map((item) => stableJson(item)).join(',')}]`;
  }

  if (value && typeof value === 'object') {
    const entries = Object.entries(value as Record<string, unknown>)
                        .sort(([left], [right]) => left.localeCompare(right));
    return `{${
        entries
            .map(
                ([key, entry]) => `${JSON.stringify(key)}:${stableJson(entry)}`)
            .join(',')}}`;
  }

  return JSON.stringify(value);
}

function canonicalStateSignature(
    seed: string, baselinePayload: Record<string, unknown>,
    deltaPayloads: Record<string, unknown>[]): string {
  const normalized = stableJson({
    seed,
    baselinePayload,
    deltaPayloads,
  });

  return createHash('sha256').update(normalized).digest('hex');
}

export function createReplayMergeService(
    baselineRepository: BaselineRepository = createBaselineRepository(),
    deltaRepository: DeltaRepository = createDeltaRepository(),
    areaStateVersionService: AreaStateVersionService =
        createAreaStateVersionService(),
    ): ReplayMergeService {
  return {
    ensureAuthoritativeBaseline(view: RevisitBaselineView):
        AuthoritativeBaselineRecord {
          const state = areaStateVersionService.ensure(
              view.areaIdentity.areaId,
              view.stateVersion.canonicalStateSignature,
          );
          return baselineRepository.upsert({
            baselineId: view.baselineId,
            areaIdentity: view.areaIdentity,
            areaStateVersion: state,
            baselinePayload: {
              orchestrationPathId: view.orchestrationPathId,
              identityHash: view.areaIdentity.identityHash,
              baselineId: view.baselineId,
            },
            updatedAt: new Date().toISOString(),
          });
        },

    applyDelta(request: ReplayDeltaRequest): ReplayMergeResult {
      const baseline =
          baselineRepository.getByAreaId(request.areaIdentity.areaId) ??
          baselineRepository.upsert({
            baselineId: request.baselineId,
            areaIdentity: request.areaIdentity,
            areaStateVersion: areaStateVersionService.ensure(
                request.areaIdentity.areaId,
                makeDeterministicCanonicalStateSignature(
                    request.areaIdentity.identityHash),
                ),
            baselinePayload: {
              identityHash: request.areaIdentity.identityHash,
              baselineId: request.baselineId,
            },
            updatedAt: new Date().toISOString(),
          });

      const duplicate = deltaRepository.getByIdempotencyKey(
          request.areaIdentity.areaId,
          request.idempotencyKey,
      );
      if (duplicate) {
        const current =
            areaStateVersionService.get(request.areaIdentity.areaId) ??
            baseline.areaStateVersion;
        return {
          areaIdentity: request.areaIdentity,
          baselineId: baseline.baselineId,
          areaStateVersion: current.areaStateVersion,
          appliedDeltaThroughSequence: current.appliedDeltaThroughSequence,
          canonicalStateSignature: current.canonicalStateSignature,
        };
      }

      areaStateVersionService.assertNotStale(
          request.areaIdentity.areaId,
          request.baseAreaStateVersion,
      );

      const appended = deltaRepository.append({
        areaId: request.areaIdentity.areaId,
        idempotencyKey: request.idempotencyKey,
        deltaPayload: request.deltaPayload,
        baseAreaStateVersion: request.baseAreaStateVersion,
      });

      const orderedDeltas =
          deltaRepository.listByAreaId(request.areaIdentity.areaId);
      const signature = canonicalStateSignature(
          request.areaIdentity.identityHash,
          baseline.baselinePayload,
          orderedDeltas.map((delta) => delta.deltaPayload),
      );

      const state = areaStateVersionService.advance(
          request.areaIdentity.areaId,
          appended.sequence,
          signature,
      );

      baselineRepository.upsert({
        ...baseline,
        areaStateVersion: state,
      });

      return {
        areaIdentity: request.areaIdentity,
        baselineId: baseline.baselineId,
        areaStateVersion: state.areaStateVersion,
        appliedDeltaThroughSequence: state.appliedDeltaThroughSequence,
        canonicalStateSignature: state.canonicalStateSignature,
      };
    },

    replay(areaId: string): ReplayMergeResult |
        null {
          const baseline = baselineRepository.getByAreaId(areaId);
          if (!baseline) {
            return null;
          }

          const state =
              areaStateVersionService.get(areaId) ?? baseline.areaStateVersion;
          return {
            areaIdentity: baseline.areaIdentity,
            baselineId: baseline.baselineId,
            areaStateVersion: state.areaStateVersion,
            appliedDeltaThroughSequence: state.appliedDeltaThroughSequence,
            canonicalStateSignature: state.canonicalStateSignature,
          };
        },
  };
}
