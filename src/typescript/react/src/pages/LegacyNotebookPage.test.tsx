import '../test-setup';

import { afterEach, describe, expect, it } from 'bun:test';
import { render, screen } from '@testing-library/react';
import { MemoryRouter, Route, Routes, useLocation } from 'react-router-dom';
import { clearStoredAuthSession, storeAuthSession } from '@banana/ui';

import { LegacyNotebookPage } from './LegacyNotebookPage';
import { clearSelectedCharacter, storeSelectedCharacter } from '../lib/gameClientFlow';

describe('LegacyNotebookPage', () => {
    function LoginRouteProbe() {
        const location = useLocation();
        return <div>{`Login route ${location.search}`}</div>;
    }

    afterEach(() => {
        clearStoredAuthSession();
        clearSelectedCharacter();
    });

    it('renders the embedded notebook baseline for authenticated users', () => {
        storeAuthSession({ token: 'token-1', subject: 'operator-1' });
        storeSelectedCharacter('Ari the Mapper');

        const { container } = render(
            <MemoryRouter initialEntries={['/legacy']}>
                <LegacyNotebookPage />
            </MemoryRouter>,
        );

        expect(screen.getByRole('link', { name: 'Legacy' })).toBeTruthy();
        expect(screen.getByText('Notebook')).toBeTruthy();
        expect(screen.getByText('Ari the Mapper')).toBeTruthy();
        expect(screen.queryByText('Open full screen notebook')).toBeNull();
        expect(screen.queryByText('Use this tab to keep the old notebook-client experience visible while building the new web routes.')).toBeNull();

        const iframe = screen.getByTitle('Legacy notebook runtime') as HTMLIFrameElement;
        expect(iframe).toBeTruthy();
        expect(iframe.getAttribute('src')).toBe('/notebooks');

        const mainElement = container.querySelector('main');
        expect(mainElement?.style.padding).toBe('0px');
    });

    it('redirects unauthenticated users to login with the legacy return target', () => {
        render(
            <MemoryRouter initialEntries={['/legacy']}>
                <Routes>
                    <Route path="/legacy" element={<LegacyNotebookPage />} />
                    <Route path="/login" element={<LoginRouteProbe />} />
                </Routes>
            </MemoryRouter>,
        );

        expect(screen.getByText('Login route ?returnTo=%2Flegacy')).toBeTruthy();
    });
});
