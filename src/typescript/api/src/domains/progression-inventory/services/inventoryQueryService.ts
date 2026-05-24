import {type InventorySnapshot, queryInventorySnapshot} from '../state/playerTruthStore.ts';

export function queryPlayerInventory(playerId: string): InventorySnapshot {
  return queryInventorySnapshot(playerId);
}
