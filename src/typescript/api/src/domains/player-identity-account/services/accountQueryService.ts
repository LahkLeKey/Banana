import {getOrCreatePlayerAccount, type PlayerAccountRecord} from '../state/accountStore.ts';

export function queryPlayerAccount(playerId: string): PlayerAccountRecord {
  return getOrCreatePlayerAccount(playerId);
}
