import {type LaunchMode, resolveEnvironmentModePolicy} from '../../../lib/contracts/environmentModePolicy.ts';
import {isRemediableLaunchGateReason, type LaunchGateReasonCode} from '../../../lib/contracts/launchGateReasonCodes.ts';
import type {RemediationTicketService} from '../services/remediationTicketService.ts';

export type VerifyLaunchAttemptInput = {
  mode: string;
  clientBuildChannel?: string;
  steamAssertion?: {
    steamId?: string;
    validationStatus?: 'valid' | 'invalid' | 'expired' | 'indeterminate';
  };
  overrideContext?: {source?: string; allowNonSteamStartup?: boolean;};
};

export type VerifyLaunchAttemptDecision = {
  decision: 'allow'|'deny'; reasonCode: LaunchGateReasonCode;
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
};

type OverrideDecision = {
  overrideApplied: boolean; overrideSource: string | null;
  allowNonSteamStartupOverride: boolean;
};

function resolveMode(
    modeRaw: string, clientBuildChannelRaw?: string): LaunchMode|null {
  const clientBuildChannel = (clientBuildChannelRaw ?? '').trim().toLowerCase();
  if (clientBuildChannel.length > 0) {
    if (clientBuildChannel.includes('steam-prod') ||
        clientBuildChannel.includes('production')) {
      return 'production-steam-package';
    }
    if (clientBuildChannel.includes('staging') ||
        clientBuildChannel.includes('uat')) {
      return 'staging-uat';
    }
    if (clientBuildChannel.includes('local') ||
        clientBuildChannel.includes('dev')) {
      return 'development';
    }
  }

  const policy = resolveEnvironmentModePolicy(modeRaw);
  return policy?.mode ?? null;
}

function resolveOverride(
    input: VerifyLaunchAttemptInput, mode: LaunchMode): OverrideDecision {
  const source = input.overrideContext?.source?.trim().toLowerCase() ?? '';
  const requestedAllowNonSteam =
      input.overrideContext?.allowNonSteamStartup === true;

  if (mode === 'production-steam-package') {
    return {
      overrideApplied: false,
      overrideSource: source.length > 0 ? source : null,
      allowNonSteamStartupOverride: false,
    };
  }

  const trustedSource = source === 'local-config' || source === 'env-var';
  if (!trustedSource || !requestedAllowNonSteam) {
    return {
      overrideApplied: false,
      overrideSource: source.length > 0 ? source : null,
      allowNonSteamStartupOverride: false,
    };
  }

  return {
    overrideApplied: true,
    overrideSource: source,
    allowNonSteamStartupOverride: true,
  };
}

export function verifyLaunchAttemptPipeline(
    input: VerifyLaunchAttemptInput,
    remediationService: RemediationTicketService,
    ): VerifyLaunchAttemptDecision {
  let reasonCode: LaunchGateReasonCode = 'OK';
  let decision: 'allow'|'deny' = 'allow';
  let effectiveMode: LaunchMode = 'development';
  let overrideApplied = false;
  let overrideSource: string|null = null;

  const resolvedMode = resolveMode(input.mode, input.clientBuildChannel);
  if (!resolvedMode) {
    reasonCode = 'UNKNOWN_MODE';
    decision = 'deny';
    effectiveMode = 'production-steam-package';
  } else {
    effectiveMode = resolvedMode;
    const policy = resolveEnvironmentModePolicy(effectiveMode);
    if (!policy) {
      reasonCode = 'UNKNOWN_MODE';
      decision = 'deny';
      effectiveMode = 'production-steam-package';
    } else {
      const overrideDecision = resolveOverride(input, effectiveMode);
      overrideApplied = overrideDecision.overrideApplied;
      overrideSource = overrideDecision.overrideSource;

      const allowNonSteamStartup = policy.allowNonSteamStartup ||
          overrideDecision.allowNonSteamStartupOverride;

      if (!allowNonSteamStartup && !input.steamAssertion?.steamId) {
        reasonCode = 'STEAM_UNAVAILABLE';
        decision = 'deny';
      } else if (
          effectiveMode === 'production-steam-package' &&
          input.steamAssertion?.validationStatus !== 'valid') {
        reasonCode = input.steamAssertion?.validationStatus === 'expired' ?
            'STALE_SESSION' :
            'OFFLINE_UNVERIFIABLE';
        decision = 'deny';
      }
    }
  }

  const isRemediable =
      decision === 'deny' && isRemediableLaunchGateReason(reasonCode);
  const remediation =
      isRemediable ? remediationService.create(reasonCode) : undefined;

  return {
    decision,
    reasonCode,
    effectiveMode,
    overrideApplied,
    overrideSource,
    isRemediable,
    remediation: remediation ? {
      remediationTicketId: remediation.remediationTicketId,
      webFlowUrl: remediation.webFlowUrl,
      requiredActions: remediation.requiredActions,
    } :
                               undefined,
    retryPolicy: {
      retryable: decision === 'deny',
      retryAfterMs: decision === 'deny' ? 10000 : 0,
      maxRecommendedAttempts: 3,
    },
  };
}
