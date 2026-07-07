import { useState } from 'react';
import { Navigate, useNavigate } from 'react-router-dom';
import {
    RouteActionButton,
    RouteActionsRow,
    RouteBody,
    RouteEyebrow,
    RouteInfoCard,
    RouteInfoGrid,
    RoutePanel,
    RouteTitle,
    clearStoredAuthSession,
    readStoredAuthSession,
} from '@banana/ui';

import { GameplayShell } from '../components/gameplay/GameplayShell';
import { buildProtectedLoginPath } from '../lib/authRouting';
import { clearSelectedCharacter, readSelectedCharacter, storeSelectedCharacter } from '../lib/gameClientFlow';

const CHARACTER_OPTIONS = [
    'Ari the Mapper',
    'Nova the Relay',
    'Kite the Systems Scout',
] as const;

const CHARACTERS_BACKGROUND = 'radial-gradient(circle at 80% 20%, rgba(14, 165, 233, 0.22), transparent 36%), linear-gradient(155deg, #020712, #071525 58%, #0c1f36)';

export function CharacterSelectPage() {
    const navigate = useNavigate();
    const session = readStoredAuthSession();
    const [selectedCharacter, setSelectedCharacter] = useState<string>(readSelectedCharacter() || CHARACTER_OPTIONS[0]);

    if (!session?.token) {
        return <Navigate to={buildProtectedLoginPath('/characters')} replace />;
    }

    const handleConfirm = () => {
        storeSelectedCharacter(selectedCharacter);
        navigate('/world-map');
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
            routeLabel="Characters"
            title="Choose your current operator"
            subtitle="Operator changes happen here only. The selection powers the locked gameplay slices."
            activeOperator={selectedCharacter}
            onSignOut={handleSignOut}
            background={CHARACTERS_BACKGROUND}
        >
            <RoutePanel width="min(1120px, 100%)">
                <RouteEyebrow color="#7dd3fc">Single active operator</RouteEyebrow>
                <RouteTitle>Choose your current operator</RouteTitle>
                <RouteBody maxWidth={860}>
                    This branch keeps a single active operator and stores it locally. The adapter seam is ready for richer account-backed behavior later, but the first slice is deliberately simple.
                </RouteBody>

                <div style={optionsGridStyle}>
                    {CHARACTER_OPTIONS.map((option, index) => {
                        const isSelected = selectedCharacter === option;
                        return (
                            <button
                                key={option}
                                type="button"
                                onClick={() => setSelectedCharacter(option)}
                                style={{
                                    ...optionCardStyle,
                                    ...(isSelected ? optionCardSelectedStyle : null),
                                }}
                            >
                                <div style={optionOrdinalStyle}>{`0${index + 1}`}</div>
                                <div style={optionNameStyle}>{option}</div>
                                <div style={optionMetaStyle}>{isSelected ? 'Current operator' : 'Select this operator'}</div>
                            </button>
                        );
                    })}
                </div>

                <RouteActionsRow marginTop={22}>
                    <RouteActionButton onClick={handleConfirm} tone="primary">
                        Confirm operator
                    </RouteActionButton>
                    <RouteActionButton onClick={() => navigate('/')} tone="neutral">
                        Back to home
                    </RouteActionButton>
                </RouteActionsRow>

                <RouteInfoGrid>
                    <RouteInfoCard title="Selection state">
                        <RouteBody maxWidth={320}>{selectedCharacter}</RouteBody>
                    </RouteInfoCard>
                    <RouteInfoCard title="Persistence mode">
                        <RouteBody maxWidth={320}>Local session storage only for this branch.</RouteBody>
                    </RouteInfoCard>
                    <RouteInfoCard title="Next route">
                        <RouteBody maxWidth={320}>Confirming the operator unlocks the world map slice.</RouteBody>
                    </RouteInfoCard>
                </RouteInfoGrid>
            </RoutePanel>
        </GameplayShell>
    );
}

const optionsGridStyle = {
    marginTop: 18,
    display: 'grid',
    gap: 12,
    gridTemplateColumns: 'repeat(auto-fit, minmax(220px, 1fr))',
};

const optionCardStyle = {
    textAlign: 'left' as const,
    borderRadius: 18,
    border: '1px solid rgba(148, 163, 184, 0.22)',
    background: 'rgba(8, 13, 28, 0.6)',
    color: '#e2e8f0',
    padding: '18px 16px',
    cursor: 'pointer',
    display: 'grid',
    gap: 8,
};

const optionCardSelectedStyle = {
    border: '1px solid rgba(20, 184, 166, 0.72)',
    background: 'rgba(13, 148, 136, 0.2)',
    transform: 'translateY(-1px)',
};

const optionOrdinalStyle = {
    fontSize: 11,
    letterSpacing: '0.12em',
    textTransform: 'uppercase' as const,
    color: '#7dd3fc',
    fontWeight: 800,
};

const optionNameStyle = {
    fontSize: 18,
    fontWeight: 800,
    color: '#f8fafc',
};

const optionMetaStyle = {
    fontSize: 13,
    color: '#cbd5e1',
};
