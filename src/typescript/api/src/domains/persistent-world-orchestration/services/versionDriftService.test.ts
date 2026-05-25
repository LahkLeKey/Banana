import {describe, expect, test} from 'bun:test';

import {createVersionDriftService} from './versionDriftService.ts';

describe('version drift service', () => {
  test(
      'reports synchronized native, api, and client versions as drift-free',
      () => {
        const service = createVersionDriftService();

        const report = service.check({
          nativeGeneratorVersion: 'county-static-v1',
          apiContractVersion: 'v1',
          clientProfileVersion: 'banana-archipelago-region-baseline-v1',
        });

        expect(report.driftDetected).toBe(false);
        expect(report.mismatches).toHaveLength(0);
      });

  test(
      'reports drift for mismatched native generator and client profile versions',
      () => {
        const service = createVersionDriftService();

        const report = service.check({
          nativeGeneratorVersion: 'county-static-v2',
          apiContractVersion: 'v1',
          clientProfileVersion: 'banana-archipelago-region-baseline-v2',
        });

        expect(report.driftDetected).toBe(true);
        expect(report.mismatches.map((mismatch) => mismatch.dimension))
            .toEqual(['native_generator', 'client_profile']);
      });
});