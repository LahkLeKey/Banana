import {conflict} from '../../../lib/errors/domainErrors.ts';
import {queryPlayerAccount} from '../services/accountQueryService.ts';

export function enforceAccountExpectedVersion(
    playerId: string,
    expectedVersion: number,
    ): void {
  const current = queryPlayerAccount(playerId);
  if (current.version !== expectedVersion) {
    throw conflict('account_version_conflict', {
      expectedVersion,
      actualVersion: current.version,
      resolutionHint: 'refresh_and_retry',
    });
  }
}
