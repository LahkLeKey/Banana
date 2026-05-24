import {getDomainChangeRecords} from '../../gameplay-session-orchestration/persistence/domainChangeRecorder.ts';
import {queryPlayerInventory} from '../../progression-inventory/services/inventoryQueryService.ts';
import {queryPlayerProgression} from '../../progression-inventory/services/progressionQueryService.ts';

export type PlayerInsightReadModel = {
  playerId: string; sessionSummary: Record<string, unknown>;
  progressionSummary: Record<string, unknown>;
  inventoryTrendSummary: Record<string, unknown>;
  noData: boolean;
  freshnessTimestamp: string;
};

export function loadPlayerInsightReadModel(playerId: string):
    PlayerInsightReadModel {
  const progression = queryPlayerProgression(playerId);
  const inventory = queryPlayerInventory(playerId);

  const playerRecords = getDomainChangeRecords().filter(
      (record) => record.actorScope.actorId === playerId);

  const xp = Number(progression.progression.xp ?? 0);
  const level = Number(progression.progression.level ?? 1);
  const hasProgressionData = xp > 0 || level > 1;
  const hasDomainData = playerRecords.length > 0 || hasProgressionData ||
      inventory.inventory.length > 0;

  const lastRecord = playerRecords[playerRecords.length - 1];
  const freshnessTimestamp = lastRecord?.recordedAt ??
      progression.freshnessTimestamp ?? inventory.freshnessTimestamp;

  return {
    playerId,
    sessionSummary: {
      changeEvents: playerRecords.length,
      latestAction: lastRecord?.actionType ?? null,
    },
    progressionSummary: progression.progression,
    inventoryTrendSummary: {
      distinctItems: inventory.inventory.length,
      totalQuantity: inventory.inventory.reduce(
          (sum, item) => sum + Number(item.quantity ?? 0), 0),
    },
    noData: !hasDomainData,
    freshnessTimestamp,
  };
}
