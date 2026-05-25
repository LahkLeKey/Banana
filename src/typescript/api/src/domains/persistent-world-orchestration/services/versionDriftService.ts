import {BANANA_ARCHIPELAGO_API_CONTRACT_VERSION, BANANA_ARCHIPELAGO_GENERATOR_VERSION, BANANA_ARCHIPELAGO_PROFILE_VERSION,} from '../worldbuildingContract.ts';

export type VersionDriftRequest = {
  nativeGeneratorVersion: string; apiContractVersion: string;
  clientProfileVersion: string;
};

export type VersionDriftMismatch = {
  dimension: 'native_generator'|'api_contract'|'client_profile';
  expected: string;
  actual: string;
};

export type VersionDriftReport = {
  canonicalVersions: {
    nativeGeneratorVersion: string; apiContractVersion: string;
    clientProfileVersion: string;
  };
  observedVersions: VersionDriftRequest;
  driftDetected: boolean;
  mismatches: VersionDriftMismatch[];
};

export interface VersionDriftService {
  check(request: VersionDriftRequest): VersionDriftReport;
}

function appendMismatch(
    mismatches: VersionDriftMismatch[],
    dimension: VersionDriftMismatch['dimension'],
    expected: string,
    actual: string,
    ): void {
  if (expected !== actual) {
    mismatches.push({dimension, expected, actual});
  }
}

export function createVersionDriftService(): VersionDriftService {
  return {
    check(request): VersionDriftReport {
      const mismatches: VersionDriftMismatch[] = [];

      appendMismatch(
          mismatches,
          'native_generator',
          BANANA_ARCHIPELAGO_GENERATOR_VERSION,
          request.nativeGeneratorVersion,
      );
      appendMismatch(
          mismatches,
          'api_contract',
          BANANA_ARCHIPELAGO_API_CONTRACT_VERSION,
          request.apiContractVersion,
      );
      appendMismatch(
          mismatches,
          'client_profile',
          BANANA_ARCHIPELAGO_PROFILE_VERSION,
          request.clientProfileVersion,
      );

      return {
        canonicalVersions: {
          nativeGeneratorVersion: BANANA_ARCHIPELAGO_GENERATOR_VERSION,
          apiContractVersion: BANANA_ARCHIPELAGO_API_CONTRACT_VERSION,
          clientProfileVersion: BANANA_ARCHIPELAGO_PROFILE_VERSION,
        },
        observedVersions: request,
        driftDetected: mismatches.length > 0,
        mismatches,
      };
    },
  };
}