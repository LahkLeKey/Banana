export type FailureContext = {
  failureCode: number; classification: 'recoverable' | 'non_recoverable';
  operatorMessage: string;
  remediationHint: string;
};

export interface FailureContextService {
  build(failureCode: number, classification: 'recoverable'|'non_recoverable'):
      FailureContext;
}

function remediationHintForCode(failureCode: number): string {
  if (failureCode === -11) {
    return 'retry_with_same_generation_inputs';
  }

  if (failureCode === -12) {
    return 'restore_dependencies_then_retry';
  }

  if (failureCode === -21) {
    return 'verify_area_identity_inputs';
  }

  if (failureCode === -22) {
    return 'upgrade_generation_contract_version';
  }

  return 'escalate_to_runtime_operator';
}

export function createFailureContextService(): FailureContextService {
  return {
    build(failureCode, classification): FailureContext {
      return {
        failureCode,
        classification,
        operatorMessage: classification === 'recoverable' ?
            'deterministic retry available' :
            'non-recoverable deterministic generation failure',
        remediationHint: remediationHintForCode(failureCode),
      };
    },
  };
}
