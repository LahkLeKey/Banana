import {randomUUID} from 'node:crypto';

import type {LaunchMode} from './contracts/environmentModePolicy.ts';
import type {LaunchGateReasonCode} from './contracts/launchGateReasonCodes.ts';

export type LaunchGateAuditEvent = {
  auditRef: string; attemptId: string; decision: 'allow' | 'deny';
  reasonCode: LaunchGateReasonCode;
  resolvedMode: LaunchMode;
  overrideApplied: boolean;
  overrideSource: string | null;
  correlationId: string;
  timestamp: string;
};

export function buildLaunchGateAuditEvent(input: {
  attemptId: string; decision: 'allow' | 'deny';
  reasonCode: LaunchGateReasonCode;
  effectiveMode: LaunchMode;
  correlationId: string;
  overrideApplied: boolean;
  overrideSource: string | null;
}): LaunchGateAuditEvent {
  return {
    auditRef: `lga_${randomUUID()}`,
    attemptId: input.attemptId,
    decision: input.decision,
    reasonCode: input.reasonCode,
    resolvedMode: input.effectiveMode,
    overrideApplied: input.overrideApplied,
    overrideSource: input.overrideSource,
    correlationId: input.correlationId,
    timestamp: new Date().toISOString(),
  };
}

export function serializeLaunchGateAuditEvent(event: LaunchGateAuditEvent):
    string {
  return JSON.stringify(event);
}
