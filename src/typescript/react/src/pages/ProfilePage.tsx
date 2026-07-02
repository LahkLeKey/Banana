import { useMemo, useState, type CSSProperties } from 'react';
import {
    RouteActionButton,
    RouteActionsRow,
    RouteBody,
    RouteEyebrow,
    RouteFieldLabel,
    RoutePanel,
    RouteShell,
    RouteTextInput,
    RouteTitle,
    clearStoredAuthSession,
    readStoredAuthSession,
} from '@banana/ui';

const DISPLAY_NAME_KEY = 'banana-profile-display-name';

function readDisplayName(): string {
    if (typeof window === 'undefined') {
        return '';
    }

    return window.localStorage.getItem(DISPLAY_NAME_KEY)?.trim() ?? '';
}

export function ProfilePage() {
    const session = useMemo(() => readStoredAuthSession(), []);
    const [displayName, setDisplayName] = useState<string>(readDisplayName());
    const [saved, setSaved] = useState(false);

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
        if (typeof window !== 'undefined') {
            window.location.assign('/login');
        }
    };

    if (!session?.token) {
        return (
            <RouteShell background={shellStyle.background as string}>
                <RoutePanel width="min(860px, 100%)">
                    <RouteEyebrow color="#93c5fd">Profile</RouteEyebrow>
                    <RouteTitle>Login required</RouteTitle>
                    <RouteBody>Sign in to set up your local profile.</RouteBody>
                    <RouteActionsRow marginTop={16}>
                        <RouteActionButton onClick={() => window.location.assign('/login')} tone="primary">
                            Go to login
                        </RouteActionButton>
                    </RouteActionsRow>
                </RoutePanel>
            </RouteShell>
        );
    }

    return (
        <RouteShell background={shellStyle.background as string}>
            <div style={profileNavStyle}>
                <RouteActionButton onClick={() => window.location.assign('/')} tone="neutral">
                    Back to runtime
                </RouteActionButton>
            </div>

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
        </RouteShell>
    );
}

const shellStyle: CSSProperties = {
    minHeight: '100dvh',
    padding: '32px 20px',
    display: 'grid',
    placeItems: 'center',
    background:
        'radial-gradient(circle at 16% 14%, rgba(56, 189, 248, 0.18), transparent 34%), radial-gradient(circle at 86% 22%, rgba(132, 204, 22, 0.2), transparent 32%), linear-gradient(160deg, #050913 0%, #0b1530 56%, #14284a 100%)',
    color: '#f8fafc',
};

const profileNavStyle: CSSProperties = {
    position: 'fixed',
    top: 20,
    left: 20,
    zIndex: 20,
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
