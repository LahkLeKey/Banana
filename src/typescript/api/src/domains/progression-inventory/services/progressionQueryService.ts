import {type ProgressionSnapshot, queryProgressionSnapshot} from '../state/playerTruthStore.ts';

export function queryPlayerProgression(playerId: string): ProgressionSnapshot {
  return queryProgressionSnapshot(playerId);
}
