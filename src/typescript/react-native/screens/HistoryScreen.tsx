// React Native HistoryScreen (slice 031).
// Surfaces last-N verdicts persisted via @banana/resilience's
// VerdictHistory backed by AsyncStorage. Killing and reopening the
// app preserves these entries (FR-031-04).

import { useEffect, useState } from 'react';
import { Pressable, SafeAreaView, ScrollView, Text, View } from 'react-native';
import { tokens, type EnsembleVerdict } from '@banana/ui/native';
import { verdictCopy } from '../lib/copy';
import { getVerdictHistory, type RecentVerdict } from '../lib/resilience-bootstrap';

export type HistoryScreenProps = {
    onBack: () => void;
};

export function HistoryScreen({ onBack }: HistoryScreenProps) {
    const [entries, setEntries] = useState<RecentVerdict[]>([]);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        let cancelled = false;
        getVerdictHistory()
            .list(25)
            .then((list) => {
                if (!cancelled) {
                    setEntries(list);
                    setLoading(false);
                }
            })
            .catch(() => {
                if (!cancelled) setLoading(false);
            });
        return () => {
            cancelled = true;
        };
    }, []);

    return (
        <SafeAreaView style={{ flex: 1, padding: tokens.space[6] }}>
            <ScrollView contentContainerStyle={{ flexGrow: 1 }}>
                <Text
                    accessibilityRole="header"
                    style={{ fontSize: tokens.font.size.lg, fontWeight: tokens.font.weight.semibold, marginBottom: tokens.space[3] }}>
                    Recent verdicts
                </Text>
                {loading ? (
                    <Text testID="history-loading" style={{ color: tokens.color.text.muted }}>Loading...</Text>
                ) : entries.length === 0 ? (
                    <Text testID="history-empty" style={{ color: tokens.color.text.muted }}>
                        No verdicts yet. Classify a sample to start your history.
                    </Text>
                ) : (
                    entries.map((entry) => (
                        <View
                            key={entry.id}
                            testID="history-item"
                            style={{
                                marginBottom: tokens.space[3],
                                padding: tokens.space[3],
                                borderRadius: tokens.radius.md,
                                backgroundColor: tokens.color.surface.default,
                                borderWidth: 1,
                                // Slice 031 exception: neutral border kept inline (no border token shipped yet).
                                borderColor: '#cbd5e1',
                            }}>
                            <Text style={{ fontSize: tokens.font.size.sm, fontWeight: tokens.font.weight.semibold, color: tokens.color.text.default }}>
                                {verdictCopy(entry.verdict as EnsembleVerdict)}
                            </Text>
                            <Text style={{ fontSize: tokens.font.size.xs, color: tokens.color.text.muted, marginTop: tokens.space[1] }}>
                                {entry.input}
                            </Text>
                        </View>
                    ))
                )}
                <Pressable
                    testID="history-back"
                    onPress={onBack}
                    style={{
                        marginTop: tokens.space[4],
                        paddingVertical: tokens.space[3],
                        borderRadius: tokens.radius.md,
                        alignItems: 'center',
                        // Slice 031 exception: sky CTA color kept; no CTA-color token shipped yet.
                        backgroundColor: '#0ea5e9',
                    }}>
                    <Text style={{ color: tokens.color.surface.default, fontWeight: tokens.font.weight.semibold }}>Back</Text>
                </Pressable>
            </ScrollView>
        </SafeAreaView>
    );
}
