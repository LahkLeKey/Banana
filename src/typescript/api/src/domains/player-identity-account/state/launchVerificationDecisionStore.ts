import type {LaunchGateReasonCode} from '../../../lib/contracts/launchGateReasonCodes.ts';

export type LaunchVerificationDecisionRecord = {
  attemptId: string; decision: 'allow' | 'deny';
  reasonCode: LaunchGateReasonCode;
  effectiveMode: 'development' | 'staging-uat' | 'production-steam-package';
  overrideApplied: boolean;
  overrideSource: string | null;
  isRemediable: boolean;
  remediation?: {
    remediationTicketId: string; webFlowUrl: string; requiredActions: string[];
  };
  retryPolicy: {
    retryable: boolean; retryAfterMs: number; maxRecommendedAttempts: number;
  };
  auditRef: string;
  decidedAt: string;
  freshnessExpiresAt: string;
};

export interface LaunchVerificationDecisionStore {
  put(record: LaunchVerificationDecisionRecord): void;
  get(attemptId: string): LaunchVerificationDecisionRecord|null;
  hasConsumedRetry(attemptId: string): boolean;
  consumeRetry(attemptId: string): void;
  reset(): void;
}

class InMemoryLaunchVerificationDecisionStore implements
    LaunchVerificationDecisionStore {
  private readonly attempts =
      new Map<string, LaunchVerificationDecisionRecord>();
  private readonly consumedRetryAttempts = new Set<string>();

  put(record: LaunchVerificationDecisionRecord): void {
    this.attempts.set(record.attemptId, record);
  }

  get(attemptId: string): LaunchVerificationDecisionRecord|null {
    return this.attempts.get(attemptId) ?? null;
  }

  hasConsumedRetry(attemptId: string): boolean {
    return this.consumedRetryAttempts.has(attemptId);
  }

  consumeRetry(attemptId: string): void {
    this.consumedRetryAttempts.add(attemptId);
  }

  reset(): void {
    this.attempts.clear();
    this.consumedRetryAttempts.clear();
  }
}

const launchVerificationDecisionStore =
    new InMemoryLaunchVerificationDecisionStore();

export function createLaunchVerificationDecisionStore():
    LaunchVerificationDecisionStore {
  return launchVerificationDecisionStore;
}

export function resetLaunchVerificationDecisionStoreForTests(): void {
  launchVerificationDecisionStore.reset();
}
