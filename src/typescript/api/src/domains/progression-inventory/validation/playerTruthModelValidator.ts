import {validationError} from '../../../lib/errors/domainErrors.ts';
import {queryPlayerAccount} from '../../player-identity-account/services/accountQueryService.ts';
import {queryPlayerInventory} from '../services/inventoryQueryService.ts';
import {queryPlayerProgression} from '../services/progressionQueryService.ts';

export function validateUnifiedPlayerTruthModel(playerId: string): {
  playerId: string; accountVersion: number; progressionRef: string;
  inventoryRef: string;
} {
  const account = queryPlayerAccount(playerId);
  const progression = queryPlayerProgression(playerId);
  const inventory = queryPlayerInventory(playerId);

  if (account.playerId !== progression.playerId ||
      account.playerId !== inventory.playerId) {
    throw validationError('player_truth_model_mismatch', {
      accountPlayerId: account.playerId,
      progressionPlayerId: progression.playerId,
      inventoryPlayerId: inventory.playerId,
    });
  }

  return {
    playerId,
    accountVersion: account.version,
    progressionRef: progression.snapshotRef,
    inventoryRef: inventory.snapshotRef,
  };
}
