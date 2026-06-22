import { createContext, useContext } from 'react';

import {
    useK3h4TrainingSession,
    type UseK3h4TrainingSessionResult,
} from './useK3h4TrainingSession';

const K3h4TrainingSessionContext =
    createContext<UseK3h4TrainingSessionResult | null>(null);

export function K3h4TrainingSessionProvider({
    children,
}: {
    children: React.ReactNode;
}) {
    const orchestrator = useK3h4TrainingSession();
    return (
        <K3h4TrainingSessionContext.Provider value={orchestrator}>
            {children}
        </K3h4TrainingSessionContext.Provider>
    );
}

export function useK3h4TrainingSessionContext(): UseK3h4TrainingSessionResult {
    const context = useContext(K3h4TrainingSessionContext);
    if (!context) {
        throw new Error(
            'useK3h4TrainingSessionContext must be used within K3h4TrainingSessionProvider',
        );
    }
    return context;
}
