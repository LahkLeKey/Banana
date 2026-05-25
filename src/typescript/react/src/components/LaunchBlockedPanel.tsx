import { useMemo } from 'react';

import type { LaunchGateMode, LaunchGateReasonCode } from '../lib/launchGateTypes';

type LaunchBlockedPanelProps = {
    mode: LaunchGateMode;
    reasonCode: LaunchGateReasonCode;
    remediationUrl?: string;
    requiredActions?: string[];
    retryable?: boolean;
    onRetry?: () => void;
    onOpenRemediation?: () => void;
};

const REASON_COPY: Record<LaunchGateReasonCode, {
    title: string;
    description: string;
}> = {
    OK: {
        title: 'Launch allowed',
        description: 'Verification passed for this launch attempt.',
    },
    UNLINKED_ACCOUNT: {
        title: 'Account link required',
        description: 'Link your Steam identity to a production Banana account.',
    },
    SUSPENDED_OR_RESTRICTED: {
        title: 'Account restricted',
        description: 'This account cannot enter gameplay until standing is restored.',
    },
    STALE_SESSION: {
        title: 'Session expired',
        description: 'Refresh Steam session evidence and retry verification.',
    },
    STEAM_UNAVAILABLE: {
        title: 'Steam verification unavailable',
        description: 'Steam context was not available for this launch attempt.',
    },
    API_UNAVAILABLE: {
        title: 'Verification service unavailable',
        description: 'The account verification dependency is temporarily unavailable.',
    },
    OFFLINE_UNVERIFIABLE: {
        title: 'Verification unavailable offline',
        description: 'Reconnect to verify account state before launching gameplay.',
    },
    UNKNOWN_MODE: {
        title: 'Unknown launch mode',
        description: 'The runtime mode could not be trusted and was denied safely.',
    },
};

function formatModeLabel(mode: LaunchGateMode): string {
    if (mode === 'production-steam-package') {
        return 'Production Steam Package';
    }
    if (mode === 'staging-uat') {
        return 'Staging UAT';
    }
    return 'Development';
}

export function LaunchBlockedPanel(props: LaunchBlockedPanelProps) {
    const {
        mode,
        reasonCode,
        remediationUrl,
        requiredActions = [],
        retryable = true,
        onRetry,
        onOpenRemediation,
    } = props;

    const copy = useMemo(() => REASON_COPY[reasonCode], [reasonCode]);

    return (
        <section
            aria-label="Launch blocked panel"
            className="rounded-lg border border-red-500/40 bg-red-950/10 p-4 text-sm"
        >
            <p className="text-xs uppercase tracking-wide text-red-200/90">
                Launch Blocked • {formatModeLabel(mode)}
            </p>
            <h2 className="mt-1 text-lg font-semibold text-red-100">{copy.title}</h2>
            <p className="mt-2 text-red-50/90">{copy.description}</p>

            {requiredActions.length > 0 && (
                <ol className="mt-3 list-decimal space-y-1 pl-5 text-red-50/95">
                    {requiredActions.map((action) => (
                        <li key={action}>{action}</li>
                    ))}
                </ol>
            )}

            <div className="mt-4 flex flex-wrap gap-2">
                <button
                    type="button"
                    disabled={!retryable}
                    onClick={() => onRetry?.()}
                    className="rounded-md border border-red-300/50 px-3 py-1.5 text-red-50 disabled:cursor-not-allowed disabled:opacity-60"
                >
                    Retry verification
                </button>
                <button
                    type="button"
                    onClick={() => {
                        onOpenRemediation?.();
                        if (remediationUrl && typeof window !== 'undefined') {
                            window.open(remediationUrl, '_blank', 'noopener,noreferrer');
                        }
                    }}
                    className="rounded-md bg-red-100 px-3 py-1.5 font-medium text-red-950"
                >
                    Open account-link flow
                </button>
            </div>
        </section>
    );
}
