import {type AreaStateVersion, AreaStateVersionSchema, type RevisitBaselineRequest, type RevisitBaselineView, RevisitBaselineViewSchema,} from '../../../lib/contracts/v1/persistentWorld.ts';
import {DomainError, persistentWorldRevisitPathConflict, persistentWorldRevisitValidationError,} from '../../../lib/errors/domainErrors.ts';
import {buildAreaIdentity, makeDeterministicBaselineId, makeDeterministicCanonicalStateSignature, normalizeRevisitBaselineRequest,} from '../normalization.ts';

import {type OrchestrationPathService} from './orchestrationPathService.ts';

export interface RevisitBaselineService {
  getAuthoritativeBaseline(rawRequest: unknown): RevisitBaselineView;
}

function buildInitialStateVersion(
    areaId: string, identityHash: string): AreaStateVersion {
  return AreaStateVersionSchema.parse({
    areaId,
    areaStateVersion: 0,
    appliedDeltaThroughSequence: 0,
    canonicalStateSignature:
        makeDeterministicCanonicalStateSignature(identityHash),
  });
}

export function createRevisitBaselineService(
    orchestrationPathService: OrchestrationPathService):
    RevisitBaselineService {
  return {
    getAuthoritativeBaseline(rawRequest: unknown): RevisitBaselineView {
      try {
        const normalizedRequest: RevisitBaselineRequest =
            normalizeRevisitBaselineRequest(rawRequest);
        const areaIdentity = buildAreaIdentity(normalizedRequest);

        const initialPath =
            orchestrationPathService.selectPath('initial-generation');
        const revisitPath =
            orchestrationPathService.selectPath('revisit-reconstruction');
        orchestrationPathService.assertSharedPath(
            initialPath.pathId, revisitPath.pathId);

        const baselineId =
            makeDeterministicBaselineId(areaIdentity.identityHash);
        const stateVersion = buildInitialStateVersion(
            areaIdentity.areaId, areaIdentity.identityHash);

        return RevisitBaselineViewSchema.parse({
          areaIdentity,
          stateVersion,
          baselineId,
          orchestrationPathId: revisitPath.pathId,
        });
      } catch (error) {
        if (error instanceof DomainError) {
          throw error;
        }

        if (error instanceof Error &&
            error.message.includes('Orchestration path mismatch')) {
          throw persistentWorldRevisitPathConflict({cause: error.message});
        }

        throw persistentWorldRevisitValidationError({
          cause: error instanceof Error ? error.message : String(error),
        });
      }
    },
  };
}
