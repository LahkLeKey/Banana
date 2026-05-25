const TEN_GB_BYTES = 10 * 1024 * 1024 * 1024;
const DEFAULT_BASELINE_METADATA_BYTES = 512;
const DEFAULT_DELTA_RECORD_BYTES = 2048;
const DEFAULT_PROGRESSION_LEDGER_BYTES = 768;

export type StorageBudgetSimulationRequest = {
  baselineAreaCount: number; deltaRecordCount: number;
  progressionLedgerEntryCount: number;
  averageBaselineMetadataBytes?: number;
  averageDeltaRecordBytes?: number;
  averageProgressionLedgerBytes?: number;
};

export type StorageBudgetDiagnostics = {
  totalBudgetBytes: number; totalUsedBytes: number; remainingBytes: number;
  usagePercent: number;
  withinBudget: boolean;
  largestConsumer:
      'baseline_metadata' | 'terrain_deltas' | 'progression_ledgers';
  components: {
    baselineMetadataBytes: number; terrainDeltaBytes: number;
    progressionLedgerBytes: number;
  };
};

export interface StorageBudgetService {
  simulate(request: StorageBudgetSimulationRequest): StorageBudgetDiagnostics;
}

function sanitizeCount(value: number): number {
  return Number.isFinite(value) && value > 0 ? Math.floor(value) : 0;
}

function sanitizeAverage(value: number|undefined, fallback: number): number {
  return value !== undefined && Number.isFinite(value) && value > 0 ?
      Math.floor(value) :
      fallback;
}

function roundUsagePercent(totalUsedBytes: number): number {
  return Math.round((totalUsedBytes / TEN_GB_BYTES) * 10000) / 100;
}

export function createStorageBudgetService(): StorageBudgetService {
  return {
    simulate(request): StorageBudgetDiagnostics {
      const baselineMetadataBytes = sanitizeCount(request.baselineAreaCount) *
          sanitizeAverage(
                                        request.averageBaselineMetadataBytes,
                                        DEFAULT_BASELINE_METADATA_BYTES,
          );
      const terrainDeltaBytes = sanitizeCount(request.deltaRecordCount) *
          sanitizeAverage(
                                    request.averageDeltaRecordBytes,
                                    DEFAULT_DELTA_RECORD_BYTES,
          );
      const progressionLedgerBytes =
          sanitizeCount(request.progressionLedgerEntryCount) *
          sanitizeAverage(
              request.averageProgressionLedgerBytes,
              DEFAULT_PROGRESSION_LEDGER_BYTES,
          );
      const totalUsedBytes =
          baselineMetadataBytes + terrainDeltaBytes + progressionLedgerBytes;
      const remainingBytes = Math.max(TEN_GB_BYTES - totalUsedBytes, 0);
      const largestConsumer = terrainDeltaBytes >= progressionLedgerBytes &&
              terrainDeltaBytes >= baselineMetadataBytes ?
          'terrain_deltas' :
          progressionLedgerBytes >= baselineMetadataBytes ?
          'progression_ledgers' :
          'baseline_metadata';

      return {
        totalBudgetBytes: TEN_GB_BYTES,
        totalUsedBytes,
        remainingBytes,
        usagePercent: roundUsagePercent(totalUsedBytes),
        withinBudget: totalUsedBytes <= TEN_GB_BYTES,
        largestConsumer,
        components: {
          baselineMetadataBytes,
          terrainDeltaBytes,
          progressionLedgerBytes,
        },
      };
    },
  };
}