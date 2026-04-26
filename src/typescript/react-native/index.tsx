// React Native entry (specs 011 + 025).
// Slice 025 swaps the chat scaffold for the customer verdict flow:
// CaptureScreen -> VerdictScreen with optional share-sheet ingest.

import { useEffect, useState } from 'react';
import { registerRootComponent } from 'expo';
import { StatusBar } from 'expo-status-bar';
import { Linking } from 'react-native';
import { CaptureScreen } from './screens/CaptureScreen';
import { VerdictScreen } from './screens/VerdictScreen';
import { registerShareListener } from './share-handler';
import type { EnsembleVerdictWithEmbedding } from './lib/api';

const API_BASE_URL = process.env.EXPO_PUBLIC_BANANA_API_BASE_URL ?? '';

function App() {
    const [verdict, setVerdict] = useState<EnsembleVerdictWithEmbedding | null>(null);
    const [draft, setDraft] = useState('');

    useEffect(() => {
        return registerShareListener(Linking, (event) => {
            setDraft(event.text);
            setVerdict(null);
        });
    }, []);

    if (verdict) {
        return (
            <>
                <StatusBar style="auto" />
                <VerdictScreen
                    payload={verdict}
                    onRetry={() => setVerdict(null)}
                />
            </>
        );
    }

    return (
        <>
            <StatusBar style="auto" />
            <CaptureScreen
                apiBaseUrl={API_BASE_URL}
                initialDraft={draft}
                onVerdict={(payload, sourceText) => {
                    setDraft(sourceText);
                    setVerdict(payload);
                }}
            />
        </>
    );
}

registerRootComponent(App);
