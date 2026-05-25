import {describe, expect, test} from 'bun:test';

import {createStorageBudgetService} from './storageBudgetService.ts';

describe('storage budget service', () => {
  test(
      'keeps simulated deltas and progression ledgers within 10 GB budget',
      () => {
        const service = createStorageBudgetService();
        const diagnostics = service.simulate({
          baselineAreaCount: 250000,
          deltaRecordCount: 180000,
          progressionLedgerEntryCount: 240000,
          averageBaselineMetadataBytes: 512,
          averageDeltaRecordBytes: 1536,
          averageProgressionLedgerBytes: 768,
        });

        expect(diagnostics.withinBudget).toBe(true);
        expect(diagnostics.remainingBytes).toBeGreaterThan(0);
        expect(diagnostics.largestConsumer).toBe('terrain_deltas');
      });

  test(
      'flags over-budget simulations and reports the dominant consumer', () => {
        const service = createStorageBudgetService();
        const diagnostics = service.simulate({
          baselineAreaCount: 500000,
          deltaRecordCount: 3000000,
          progressionLedgerEntryCount: 4000000,
          averageBaselineMetadataBytes: 1024,
          averageDeltaRecordBytes: 3072,
          averageProgressionLedgerBytes: 1024,
        });

        expect(diagnostics.withinBudget).toBe(false);
        expect(diagnostics.remainingBytes).toBe(0);
        expect(diagnostics.usagePercent).toBeGreaterThan(100);
        expect(diagnostics.largestConsumer).toBe('terrain_deltas');
      });
});