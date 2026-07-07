import { useMemo, useState } from 'react';
import { Navigate, useNavigate } from 'react-router-dom';
import {
    RouteActionButton,
    RouteEyebrow,
    RouteActionsRow,
    RouteBody,
    RouteInfoCard,
    RoutePanel,
    RouteTitle,
    readStoredAuthSession,
    clearStoredAuthSession,
} from '@banana/ui';

import { GameplayShell } from '../components/gameplay/GameplayShell';
import { buildProtectedLoginPath } from '../lib/authRouting';
import {
    clearSelectedCharacter,
    readSelectedCharacter,
    storeSelectedCharacter,
} from '../lib/gameClientFlow';

const HOME_BACKGROUND = 'radial-gradient(circle at 12% 10%, rgba(20, 184, 166, 0.2), transparent 30%), radial-gradient(circle at 88% 16%, rgba(56, 189, 248, 0.16), transparent 34%), linear-gradient(155deg, #03101f 0%, #08172a 46%, #0d1f35 100%)';

type CharacterOption = {
    readonly name: string;
    readonly role: string;
    readonly origin: string;
    readonly specialty: string;
    readonly bio: string;
    readonly accent: string;
};

const CHARACTER_OPTIONS: readonly CharacterOption[] = [
    {
        name: 'Ari the Mapper',
        role: 'Pathfinder',
        origin: 'North Reach',
        specialty: 'Fog route decoding',
        bio: 'Ari specializes in reading unstable map geometry and finding safe progression lanes.',
        accent: '#38bdf8',
    },
    {
        name: 'Nova the Relay',
        role: 'Signal runner',
        origin: 'Relay Coast',
        specialty: 'Comms continuity',
        bio: 'Nova keeps squad telemetry intact across noisy sectors and contested uplinks.',
        accent: '#22c55e',
    },
    {
        name: 'Kite the Systems Scout',
        role: 'Systems scout',
        origin: 'Delta Basin',
        specialty: 'Anomaly inspection',
        bio: 'Kite pushes into unknown zones, confirms state changes, and tags high-value objectives.',
        accent: '#f59e0b',
    },
];

export function HomePage() {
    const navigate = useNavigate();
    const session = readStoredAuthSession();

    if (!session?.token) {
        return <Navigate to={buildProtectedLoginPath('/')} replace />;
    }

    const activeOperator = readSelectedCharacter();
    const [focusedName, setFocusedName] = useState(activeOperator || CHARACTER_OPTIONS[0].name);

    const focusedCharacter = useMemo(
        () => CHARACTER_OPTIONS.find((option) => option.name === focusedName) ?? CHARACTER_OPTIONS[0],
        [focusedName],
    );
    const isCompact = typeof window !== 'undefined' && window.innerWidth < 980;
    const splitLayoutStyle = {
        ...layoutSplitStyle,
        gridTemplateColumns: isCompact ? '1fr' : '2fr 1fr',
    };

    const handleSignOut = () => {
        clearStoredAuthSession();
        clearSelectedCharacter();
        if (typeof window !== 'undefined') {
            window.location.assign('/login');
        }
    };

    const handleActivateCharacter = () => {
        storeSelectedCharacter(focusedCharacter.name);
        navigate('/world-map');
    };

    return (
        <GameplayShell
            routeLabel="Home"
            title="Character select"
            subtitle={activeOperator ? `Active character: ${activeOperator}` : 'Select a character to begin.'}
            activeOperator={activeOperator || undefined}
            onSignOut={handleSignOut}
            background={HOME_BACKGROUND}
        >
            <RoutePanel width="min(1120px, 100%)">
                <RouteEyebrow color="#7dd3fc">Operator staging</RouteEyebrow>
                <RouteTitle>Choose your character.</RouteTitle>

                <div style={splitLayoutStyle}>
                    <section style={previewPaneStyle}>
                        <div style={previewBadgeRowStyle}>
                            <span style={previewRoleBadgeStyle}>{focusedCharacter.role}</span>
                            <span style={{ ...previewStatusStyle, color: focusedCharacter.name === activeOperator ? '#86efac' : '#fcd34d' }}>
                                {focusedCharacter.name === activeOperator ? 'Active' : 'Ready'}
                            </span>
                        </div>
                        <h2 style={previewTitleStyle}>{focusedCharacter.name}</h2>
                        <p style={previewBioStyle}>{focusedCharacter.bio}</p>

                        <div style={previewMetaGridStyle}>
                            <article style={previewMetaCardStyle}>
                                <span style={previewMetaLabelStyle}>Origin</span>
                                <strong style={previewMetaValueStyle}>{focusedCharacter.origin}</strong>
                            </article>
                            <article style={previewMetaCardStyle}>
                                <span style={previewMetaLabelStyle}>Specialty</span>
                                <strong style={previewMetaValueStyle}>{focusedCharacter.specialty}</strong>
                            </article>
                        </div>

                        <div style={{ ...previewGlowStyle, boxShadow: `0 0 0 1px ${focusedCharacter.accent}44, 0 0 42px ${focusedCharacter.accent}26` }} />
                    </section>

                    <aside style={listPaneStyle}>
                        <div style={listHeaderStyle}>
                            <h3 style={listTitleStyle}>Characters</h3>
                            <span style={listCountStyle}>{CHARACTER_OPTIONS.length}</span>
                        </div>

                        <div style={characterListStyle}>
                            {CHARACTER_OPTIONS.map((option) => {
                                const isFocused = option.name === focusedCharacter.name;
                                const isActive = option.name === activeOperator;
                                return (
                                    <button
                                        key={option.name}
                                        type="button"
                                        onClick={() => setFocusedName(option.name)}
                                        style={{
                                            ...characterRowStyle,
                                            ...(isFocused ? characterRowFocusedStyle : null),
                                        }}
                                    >
                                        <span style={characterNameStyle}>{option.name}</span>
                                        <span style={characterRoleStyle}>{option.role}</span>
                                        {isActive ? <span style={characterActiveStyle}>Active</span> : null}
                                    </button>
                                );
                            })}
                        </div>

                        <RouteActionsRow marginTop={12}>
                            <RouteActionButton onClick={handleActivateCharacter} tone="primary">
                                Play as {focusedCharacter.name.split(' ')[0]}
                            </RouteActionButton>
                            <RouteActionButton onClick={() => navigate('/characters')} tone="neutral">
                                New character
                            </RouteActionButton>
                        </RouteActionsRow>
                    </aside>
                </div>

                <RouteInfoCard title="Current status">
                    <RouteBody maxWidth={860}>
                        {activeOperator ? `${activeOperator} is active and world map access is unlocked.` : 'No active character yet. Choose one on the right to continue.'}
                    </RouteBody>
                </RouteInfoCard>
            </RoutePanel>
        </GameplayShell>
    );
}

const layoutSplitStyle = {
    display: 'grid',
    gridTemplateColumns: '2fr 1fr',
    gap: 14,
    marginTop: 18,
};

const previewPaneStyle = {
    borderRadius: 18,
    border: '1px solid rgba(148, 163, 184, 0.18)',
    background: 'rgba(8, 13, 28, 0.58)',
    padding: 20,
    display: 'grid',
    gap: 14,
    position: 'relative' as const,
    overflow: 'hidden',
};

const previewBadgeRowStyle = {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    gap: 10,
};

const previewRoleBadgeStyle = {
    fontSize: 11,
    fontWeight: 800,
    textTransform: 'uppercase' as const,
    letterSpacing: '0.1em',
    color: '#7dd3fc',
    border: '1px solid rgba(56, 189, 248, 0.35)',
    borderRadius: 999,
    padding: '5px 10px',
    background: 'rgba(8, 47, 73, 0.35)',
};

const previewStatusStyle = {
    fontSize: 11,
    fontWeight: 800,
    textTransform: 'uppercase' as const,
    letterSpacing: '0.08em',
};

const previewTitleStyle = {
    margin: 0,
    fontSize: 34,
    lineHeight: 1.1,
    color: '#f8fafc',
};

const previewBioStyle = {
    margin: 0,
    fontSize: 16,
    lineHeight: 1.6,
    color: '#cbd5e1',
    maxWidth: 620,
};

const previewMetaGridStyle = {
    display: 'grid',
    gridTemplateColumns: 'repeat(2, minmax(0, 1fr))',
    gap: 10,
};

const previewMetaCardStyle = {
    borderRadius: 12,
    border: '1px solid rgba(148, 163, 184, 0.2)',
    background: 'rgba(2, 6, 23, 0.45)',
    padding: 12,
    display: 'grid',
    gap: 5,
};

const previewMetaLabelStyle = {
    fontSize: 10,
    fontWeight: 800,
    letterSpacing: '0.12em',
    textTransform: 'uppercase' as const,
    color: '#7dd3fc',
};

const previewMetaValueStyle = {
    fontSize: 15,
    color: '#f8fafc',
};

const previewGlowStyle = {
    position: 'absolute' as const,
    right: -60,
    bottom: -60,
    width: 220,
    height: 220,
    borderRadius: '50%',
    background: 'radial-gradient(circle, rgba(56, 189, 248, 0.18) 0%, rgba(8, 13, 28, 0) 72%)',
    pointerEvents: 'none' as const,
};

const listPaneStyle = {
    borderRadius: 18,
    border: '1px solid rgba(148, 163, 184, 0.18)',
    background: 'rgba(8, 13, 28, 0.58)',
    padding: 16,
    display: 'grid',
    gap: 10,
    alignContent: 'start',
};

const listHeaderStyle = {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    gap: 8,
};

const listTitleStyle = {
    margin: 0,
    fontSize: 16,
    color: '#f8fafc',
};

const listCountStyle = {
    fontSize: 12,
    color: '#93c5fd',
    fontWeight: 700,
};

const characterListStyle = {
    display: 'grid',
    gap: 8,
};

const characterRowStyle = {
    borderRadius: 12,
    border: '1px solid rgba(148, 163, 184, 0.22)',
    background: 'rgba(2, 6, 23, 0.45)',
    color: '#e2e8f0',
    padding: '10px 12px',
    textAlign: 'left' as const,
    cursor: 'pointer',
    display: 'grid',
    gap: 3,
};

const characterRowFocusedStyle = {
    border: '1px solid rgba(45, 212, 191, 0.5)',
    background: 'rgba(13, 148, 136, 0.2)',
};

const characterNameStyle = {
    fontSize: 14,
    fontWeight: 700,
};

const characterRoleStyle = {
    fontSize: 12,
    color: '#93c5fd',
};

const characterActiveStyle = {
    marginTop: 2,
    fontSize: 10,
    letterSpacing: '0.08em',
    textTransform: 'uppercase' as const,
    color: '#86efac',
    fontWeight: 800,
};
