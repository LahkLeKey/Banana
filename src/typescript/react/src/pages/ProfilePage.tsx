import { useMemo, useState, type CSSProperties } from 'react';
import { Navigate } from 'react-router-dom';
import {
    RouteActionButton,
    RouteActionsRow,
    RouteBody,
    RouteEyebrow,
    RouteFieldLabel,
    RoutePanel,
    RouteTextInput,
    RouteTitle,
    clearStoredAuthSession,
    readStoredAuthSession,
} from '@banana/ui';

import { GameplayShell } from '../components/gameplay/GameplayShell';
import { buildProtectedLoginPath } from '../lib/authRouting';
import { clearSelectedCharacter, readSelectedCharacter } from '../lib/gameClientFlow';
const DISPLAY_NAME_KEY = 'banana-profile-display-name';

function readDisplayName(): string {
    if (typeof window === 'undefined') {
        return '';
    }

    return window.localStorage.getItem(DISPLAY_NAME_KEY)?.trim() ?? '';
}

export function ProfilePage() {
    const session = useMemo(() => readStoredAuthSession(), []);
    const activeOperator = readSelectedCharacter();
    const [displayName, setDisplayName] = useState<string>(readDisplayName());
    const [saved, setSaved] = useState(false);

    if (!session?.token) {
        return <Navigate to={buildProtectedLoginPath('/profile')} replace />;
    }
    const handleSave = () => {
        if (typeof window === 'undefined') {
            return;
        }

        const normalized = displayName.trim();
        window.localStorage.setItem(DISPLAY_NAME_KEY, normalized);
        setSaved(true);
        window.setTimeout(() => setSaved(false), 1500);
    };

    const handleSignOut = () => {
        clearStoredAuthSession();
        clearSelectedCharacter();
        if (typeof window !== 'undefined') {
            window.location.assign('/login');
        }
    };

    return (
        <GameplayShell
            routeLabel="Profile"
            title="Basic profile"
            subtitle="Local profile details for the current authenticated browser session."
            activeOperator={activeOperator || undefined}
            onSignOut={handleSignOut}
            background={shellStyle.background as string}
        >
            <RoutePanel width="min(920px, 100%)">
                <RouteEyebrow color="#86efac">Authenticated</RouteEyebrow>
                <RouteTitle>Basic Profile</RouteTitle>
                <RouteBody>
                    This is a local starter profile while we finish the full account pipeline.
                </RouteBody>

                <div style={identityBoxStyle}>
                    <strong>Identity subject</strong>
                    <div style={identityValueStyle}>{session.subject}</div>
                </div>

                <RouteFieldLabel>Display name</RouteFieldLabel>
                <RouteTextInput
                    value={displayName}
                    onChange={setDisplayName}
                    maxLength={40}
                    placeholder="Choose a display name"
                />

                <RouteActionsRow marginTop={16}>
                    <RouteActionButton onClick={handleSave} tone="primary">
                        Save profile
                    </RouteActionButton>
                    <RouteActionButton onClick={handleSignOut} tone="danger">
                        Sign out
                    </RouteActionButton>
                </RouteActionsRow>

                {saved ? <p style={savedStyle}>Profile saved locally.</p> : null}
            </RoutePanel>
        </GameplayShell>
    );
}

const shellStyle: CSSProperties = {
    minHeight: '100dvh',
    background:
        'radial-gradient(circle at 16% 14%, rgba(56, 189, 248, 0.18), transparent 34%), radial-gradient(circle at 86% 22%, rgba(132, 204, 22, 0.2), transparent 32%), linear-gradient(160deg, #050913 0%, #0b1530 56%, #14284a 100%)',
    color: '#f8fafc',
};

const identityBoxStyle: CSSProperties = {
    margin: '16px 0',
    border: '1px solid rgba(148, 163, 184, 0.3)',
    borderRadius: 12,
    padding: 12,
    background: 'rgba(15, 23, 42, 0.5)',
};

const identityValueStyle: CSSProperties = {
    marginTop: 6,
    fontFamily: 'monospace',
    color: '#cbd5e1',
    wordBreak: 'break-all',
};

const savedStyle: CSSProperties = {
    marginTop: 12,
    color: '#86efac',
};
