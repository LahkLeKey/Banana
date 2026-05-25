import '../test-setup';

import { beforeAll, describe, expect, it, mock } from 'bun:test';
import { render, screen } from '@testing-library/react';

beforeAll(() => {
    mock.module('@banana/ui', () => ({
        SessionRoomPage: () => null,
    }));
});

const { SessionRoomPage } = await import('./SessionRoomPage');

describe('SessionRoomPage launch mode disclosure', () => {
    it('shows disclosure banner for non-production launch mode', () => {
        render(
            <SessionRoomPage
                launchModeOverride="development"
                renderSharedPage={false}
            />,
        );

        expect(screen.getByRole('status', { name: 'Launch mode disclosure' }))
            .toBeTruthy();
        expect(screen.getByText(
            'Launch mode: development. Production Steam launch gating is not fully enforced in this mode.'))
            .toBeTruthy();
    });

    it('hides disclosure banner in production launch mode', () => {
        render(
            <SessionRoomPage
                launchModeOverride="production-steam-package"
                renderSharedPage={false}
            />,
        );

        expect(screen.queryByRole('status', { name: 'Launch mode disclosure' }))
            .toBeNull();
    });
});
