import '../test-setup';

import { describe, expect, it, mock } from 'bun:test';
import { render, screen } from '@testing-library/react';
import userEvent from '@testing-library/user-event';

import { LaunchBlockedPanel } from './LaunchBlockedPanel';

describe('LaunchBlockedPanel', () => {
    it('renders reason copy and required remediation actions', () => {
        render(
            <LaunchBlockedPanel
                mode="production-steam-package"
                reasonCode="UNLINKED_ACCOUNT"
                requiredActions={[
                    'Sign in to Banana account portal',
                    'Link Steam identity',
                    'Retry launch verification',
                ]}
            />,
        );

        expect(screen.getByText('Account link required')).toBeTruthy();
        expect(screen.getByText(
            'Link your Steam identity to a production Banana account.'))
            .toBeTruthy();
        expect(screen.getByText('Sign in to Banana account portal')).toBeTruthy();
        expect(screen.getByText('Link Steam identity')).toBeTruthy();
        expect(screen.getByText('Retry launch verification')).toBeTruthy();
    });

    it('wires retry and remediation actions to callbacks', async () => {
        const retrySpy = mock(() => { });
        const remediationSpy = mock(() => { });
        const user = userEvent.setup();

        render(
            <LaunchBlockedPanel
                mode="development"
                reasonCode="STEAM_UNAVAILABLE"
                retryable
                onRetry={retrySpy}
                onOpenRemediation={remediationSpy}
            />,
        );

        await user.click(screen.getByRole('button', { name: 'Retry verification' }));
        await user.click(
            screen.getByRole('button', { name: 'Open account-link flow' }));

        expect(retrySpy).toHaveBeenCalledTimes(1);
        expect(remediationSpy).toHaveBeenCalledTimes(1);
    });

    it('disables retry button when retry is not allowed', () => {
        render(
            <LaunchBlockedPanel
                mode="production-steam-package"
                reasonCode="SUSPENDED_OR_RESTRICTED"
                retryable={false}
            />,
        );

        const retryButton =
            screen.getByRole('button', { name: 'Retry verification' });
        expect((retryButton as HTMLButtonElement).disabled).toBe(true);
    });
});
