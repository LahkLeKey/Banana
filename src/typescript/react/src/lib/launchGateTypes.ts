export type LaunchGateMode =
    'development'|'staging-uat'|'production-steam-package';

export type LaunchGateDecision = 'allow'|'deny';

export type LaunchGateReasonCode =
    'OK'|'UNLINKED_ACCOUNT'|'SUSPENDED_OR_RESTRICTED'|'STALE_SESSION'|
    'STEAM_UNAVAILABLE'|'API_UNAVAILABLE'|'OFFLINE_UNVERIFIABLE'|'UNKNOWN_MODE';

export type LaunchGateRetryPolicy = {
  retryable: boolean; retryAfterMs: number; maxRecommendedAttempts: number;
};

export type LaunchGateRemediation = {
  remediationTicketId: string; webFlowUrl: string; requiredActions: string[];
};

export type LaunchGateVerifyResponse = {
  decision: LaunchGateDecision; reasonCode: LaunchGateReasonCode;
  effectiveMode: LaunchGateMode;
  isRemediable: boolean;
  remediation?: LaunchGateRemediation; retryPolicy: LaunchGateRetryPolicy;
  auditRef: string;
};

export type LaunchGateStatusResponse = {
  attemptId: string; decision: LaunchGateDecision;
  reasonCode: LaunchGateReasonCode;
  decidedAt: string;
  effectiveMode: LaunchGateMode;
  freshnessExpiresAt: string;
  remediation?: LaunchGateRemediation;
};
