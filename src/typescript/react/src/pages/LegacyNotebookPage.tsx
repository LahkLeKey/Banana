import { Navigate } from 'react-router-dom';
import {
    clearStoredAuthSession,
    readStoredAuthSession,
} from '@banana/ui';

import { GameplayShell } from '../components/gameplay/GameplayShell';
import { buildProtectedLoginPath } from '../lib/authRouting';
import { clearSelectedCharacter, readSelectedCharacter } from '../lib/gameClientFlow';

const LEGACY_BACKGROUND =
    'radial-gradient(circle at 10% 12%, rgba(20, 184, 166, 0.18), transparent 28%), radial-gradient(circle at 88% 14%, rgba(14, 165, 233, 0.18), transparent 34%), linear-gradient(155deg, #02050c 0%, #071322 45%, #0a1b2d 100%)';

export function LegacyNotebookPage() {
    const session = readStoredAuthSession();

    if (!session?.token) {
        return <Navigate to={buildProtectedLoginPath('/legacy')} replace />;
    }

    const activeOperator = readSelectedCharacter();

    const handleSignOut = () => {
        clearStoredAuthSession();
        clearSelectedCharacter();
        if (typeof window !== 'undefined') {
            window.location.assign('/login');
        }
    };

    return (
        <GameplayShell
            routeLabel="Legacy"
            title="Notebook"
            subtitle=""
            activeOperator={activeOperator || undefined}
            onSignOut={handleSignOut}
            background={LEGACY_BACKGROUND}
            contentStyle={{ padding: 0 }}
        >
            <div style={embedFrameStyle}>
                <iframe
                    title="Legacy notebook runtime"
                    src="/notebooks"
                    style={iframeStyle}
                    loading="lazy"
                />
            </div>
        </GameplayShell>
    );
}

const embedFrameStyle = {
    width: '100%',
    height: 'calc(100dvh - 92px)',
    overflow: 'hidden' as const,
    background: '#02050c',
};

const iframeStyle = {
    width: '100%',
    height: '100%',
    border: 0,
    background: '#02050c',
};
