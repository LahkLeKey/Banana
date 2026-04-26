// React Native VerdictScreen (slice 025).
// Renders canonical verdict copy + escalation cue + lazy-loaded
// fingerprint via the shared native EscalationPanel.

import { Pressable, SafeAreaView, ScrollView, Text, View } from 'react-native';
import { EscalationPanel, type EmbeddingSummary } from '@banana/ui/native';
import type { EnsembleVerdictWithEmbedding } from '../lib/api';
import { RETRY_BUTTON_COPY, verdictCopy } from '../lib/copy';

export type VerdictScreenProps = {
    payload: EnsembleVerdictWithEmbedding;
    onRetry: () => void;
};

export function VerdictScreen({ payload, onRetry }: VerdictScreenProps) {
    const { verdict, embedding } = payload;
    const copy = verdictCopy(verdict);

    async function loadSummary(): Promise<EmbeddingSummary> {
        return { embedding };
    }

    return (
        <SafeAreaView style={{ flex: 1, padding: 24 }}>
            <ScrollView contentContainerStyle={{ flexGrow: 1 }}>
                <Text style={{ fontSize: 22, fontWeight: '600', marginBottom: 16 }} accessibilityRole="header">
                    Verdict
                </Text>
                <View
                    testID="verdict-surface"
                    style={{ padding: 16, borderRadius: 12, backgroundColor: verdict.label === 'banana' ? '#fef9c3' : '#e2e8f0' }}>
                    <Text testID="verdict-copy" style={{ fontSize: 18, fontWeight: '600', color: '#1f2937' }}>
                        {copy}
                    </Text>
                    {verdict.did_escalate ? (
                        <View testID="verdict-escalation-cue" style={{ marginTop: 12 }}>
                            <EscalationPanel loadSummary={loadSummary} />
                        </View>
                    ) : null}
                </View>
                <Pressable
                    testID="verdict-retry"
                    onPress={onRetry}
                    style={{ marginTop: 16, paddingVertical: 12, borderRadius: 8, alignItems: 'center', backgroundColor: '#0ea5e9' }}>
                    <Text style={{ color: '#ffffff', fontWeight: '600' }}>{RETRY_BUTTON_COPY}</Text>
                </Pressable>
            </ScrollView>
        </SafeAreaView>
    );
}
