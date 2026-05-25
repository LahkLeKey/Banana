import {randomUUID} from 'node:crypto';

import {type EnvironmentModePolicy, type LaunchMode} from '../../../lib/contracts/environmentModePolicy.ts';
import {type LaunchGateReasonCode} from '../../../lib/contracts/launchGateReasonCodes.ts';
import {buildLaunchGateAuditEvent} from '../../../lib/launchGateAudit.ts';
import {verifyLaunchAttemptPipeline} from '../pipelines/verifyLaunchAttemptPipeline.ts';
import {createLaunchVerificationDecisionStore, type LaunchVerificationDecisionStore, resetLaunchVerificationDecisionStoreForTests,} from '../state/launchVerificationDecisionStore.ts';

import {createRemediationTicketService, type RemediationTicketService, resetRemediationTicketServiceForTests} from './remediationTicketService.ts';

export type LaunchGateVerifyInput = {
  attemptId?: string; mode: string;
  clientBuildChannel?: string;
  steamAssertion?: {
    steamId?: string;
    validationStatus?: 'valid' | 'invalid' | 'expired' | 'indeterminate';
  };
  overrideContext?: {source?: string; allowNonSteamStartup?: boolean;};
};

export type LaunchGateStatus = {
  attemptId: string; decision: 'allow' | 'deny';
  reasonCode: LaunchGateReasonCode;
  effectiveMode: LaunchMode;
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

export interface LaunchGateVerificationService {
  verify(input: LaunchGateVerifyInput, correlationId: string): LaunchGateStatus;
  getStatus(attemptId: string): LaunchGateStatus|null;
  recordRetryIntent(input: {
    attemptId: string; priorReasonCode: string; clientObservedAt: string;
  }): {
    retryAccepted: boolean; nextAttemptId: string;
    requiresFreshVerification: boolean;
  };
}

class InMemoryLaunchGateVerificationService implements
    LaunchGateVerificationService {
  private readonly decisionStore: LaunchVerificationDecisionStore;
  private readonly remediationService: RemediationTicketService;

  constructor(
      remediationService: RemediationTicketService,
      decisionStore: LaunchVerificationDecisionStore,
  ) {
    this.remediationService = remediationService;
    this.decisionStore = decisionStore;
  }

  verify(input: LaunchGateVerifyInput, correlationId: string):
      LaunchGateStatus {
    const attemptId = input.attemptId?.trim() || randomUUID();

    const decision = verifyLaunchAttemptPipeline(
        {
          mode: input.mode,
          clientBuildChannel: input.clientBuildChannel,
          steamAssertion: input.steamAssertion,
          overrideContext: input.overrideContext,
        },
        this.remediationService,
    );

    const auditEvent = buildLaunchGateAuditEvent({
      attemptId,
      decision: decision.decision,
      reasonCode: decision.reasonCode,
      effectiveMode: decision.effectiveMode,
      correlationId,
      overrideApplied: decision.overrideApplied,
      overrideSource: decision.overrideSource,
    });

    const status: LaunchGateStatus = {
      attemptId,
      decision: decision.decision,
      reasonCode: decision.reasonCode,
      effectiveMode: decision.effectiveMode,
      overrideApplied: decision.overrideApplied,
      overrideSource: decision.overrideSource,
      isRemediable: decision.isRemediable,
      remediation: decision.remediation,
      retryPolicy: decision.retryPolicy,
      auditRef: auditEvent.auditRef,
      decidedAt: auditEvent.timestamp,
      freshnessExpiresAt: new Date(Date.now() + 5 * 60_000).toISOString(),
    };

    this.decisionStore.put(status);
    return status;
  }

  getStatus(attemptId: string): LaunchGateStatus|null {
    return this.decisionStore.get(attemptId);
  }

  recordRetryIntent(input: {
    attemptId: string; priorReasonCode: string; clientObservedAt: string;
  }): {
    retryAccepted: boolean; nextAttemptId: string;
    requiresFreshVerification: boolean;
  } {
    const observedAtMs = Date.parse(input.clientObservedAt);
    if (Number.isNaN(observedAtMs)) {
      return {
        retryAccepted: false,
        nextAttemptId: randomUUID(),
        requiresFreshVerification: true,
      };
    }

    if (this.decisionStore.hasConsumedRetry(input.attemptId)) {
      return {
        retryAccepted: false,
        nextAttemptId: randomUUID(),
        requiresFreshVerification: true,
      };
    }

    const existing = this.decisionStore.get(input.attemptId);
    if (!existing) {
      return {
        retryAccepted: false,
        nextAttemptId: randomUUID(),
        requiresFreshVerification: true,
      };
    }

    if (existing.decision !== 'deny' || !existing.retryPolicy.retryable) {
      return {
        retryAccepted: false,
        nextAttemptId: randomUUID(),
        requiresFreshVerification: true,
      };
    }

    if (existing.reasonCode !== input.priorReasonCode) {
      return {
        retryAccepted: false,
        nextAttemptId: randomUUID(),
        requiresFreshVerification: true,
      };
    }

    this.decisionStore.consumeRetry(input.attemptId);

    return {
      retryAccepted: true,
      nextAttemptId: randomUUID(),
      requiresFreshVerification: true,
    };
  }

  reset(): void {
    this.decisionStore.reset();
  }
}

const launchGateVerificationService = new InMemoryLaunchGateVerificationService(
    createRemediationTicketService(),
    createLaunchVerificationDecisionStore(),
);

export function createLaunchGateVerificationService():
    LaunchGateVerificationService {
  return launchGateVerificationService;
}

export function resetLaunchGateVerificationServiceForTests(): void {
  launchGateVerificationService.reset();
  resetLaunchVerificationDecisionStoreForTests();
  resetRemediationTicketServiceForTests();
}
