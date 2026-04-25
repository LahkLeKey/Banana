// React Native entry (spec 011). Consumes only cross-platform exports from
// @banana/ui. Web-only Tailwind components are forbidden in this tree.
import { useCallback, useEffect, useRef, useState } from 'react';
import { registerRootComponent } from 'expo';
import { StatusBar } from 'expo-status-bar';
import {
    ActivityIndicator,
    Pressable,
    SafeAreaView,
    ScrollView,
    Text,
    TextInput,
    View,
} from 'react-native';
import {
    BananaBadge,
    ChatMessageBubble,
    RipenessLabel,
    type ChatMessage,
    type ChatSession,
} from '@banana/ui/native';

const API_BASE_URL = process.env.EXPO_PUBLIC_BANANA_API_BASE_URL ?? '';
const PLATFORM = 'mobile';

type CreateSessionResponse = {
    session: ChatSession;
    welcome_message: ChatMessage;
};

type SendMessageResponse = {
    session_id: string;
    duplicate: boolean;
    user_message: ChatMessage;
    assistant_message: ChatMessage;
};

type ErrorPayload = {
    error?: {
        message?: string;
    };
};

function mergeMessages(existing: ChatMessage[], incoming: ChatMessage[]): ChatMessage[] {
    const merged = [...existing];
    const ids = new Set(existing.map((message) => message.id));
    for (const message of incoming) {
        if (!ids.has(message.id)) {
            merged.push(message);
            ids.add(message.id);
        }
    }
    return merged;
}

async function parseApiError(response: Response): Promise<string> {
    try {
        const payload = (await response.json()) as ErrorPayload;
        return payload.error?.message ?? `request failed (${response.status})`;
    } catch {
        return `request failed (${response.status})`;
    }
}

function App() {
    const [session, setSession] = useState<ChatSession | null>(null);
    const [messages, setMessages] = useState<ChatMessage[]>([]);
    const [draft, setDraft] = useState('');
    const [chatError, setChatError] = useState<string | null>(null);
    const [isBootstrapping, setIsBootstrapping] = useState(false);
    const [isSending, setIsSending] = useState(false);
    const messageCounter = useRef(0);

    useEffect(() => {
        if (!API_BASE_URL) {
            setChatError('set EXPO_PUBLIC_BANANA_API_BASE_URL to enable chat');
            return;
        }

        let cancelled = false;
        async function bootstrapSession() {
            setIsBootstrapping(true);
            setChatError(null);
            try {
                const response = await fetch(`${API_BASE_URL}/chat/sessions`, {
                    method: 'POST',
                    headers: { 'content-type': 'application/json' },
                    body: JSON.stringify({
                        platform: PLATFORM,
                        metadata: { source: 'react-native-portal' },
                    }),
                });

                if (!response.ok) {
                    throw new Error(await parseApiError(response));
                }

                const payload = (await response.json()) as CreateSessionResponse;
                if (!cancelled) {
                    setSession(payload.session);
                    setMessages([payload.welcome_message]);
                }
            } catch (error: unknown) {
                if (!cancelled) {
                    setChatError(error instanceof Error ? error.message : 'failed to create chat session');
                }
            } finally {
                if (!cancelled) {
                    setIsBootstrapping(false);
                }
            }
        }

        void bootstrapSession();
        return () => {
            cancelled = true;
        };
    }, []);

    const sendMessage = useCallback(async () => {
        if (!session || !API_BASE_URL) {
            return;
        }

        const content = draft.trim();
        if (!content) {
            return;
        }

        setDraft('');
        setIsSending(true);
        setChatError(null);

        try {
            messageCounter.current += 1;
            const response = await fetch(`${API_BASE_URL}/chat/sessions/${session.id}/messages`, {
                method: 'POST',
                headers: { 'content-type': 'application/json' },
                body: JSON.stringify({
                    content,
                    client_message_id: `${PLATFORM}-${session.id}-${messageCounter.current}`,
                }),
            });

            if (!response.ok) {
                throw new Error(await parseApiError(response));
            }

            const payload = (await response.json()) as SendMessageResponse;
            setMessages((existing) =>
                mergeMessages(existing, [payload.user_message, payload.assistant_message]),
            );
        } catch (error: unknown) {
            setChatError(error instanceof Error ? error.message : 'failed to send chat message');
        } finally {
            setIsSending(false);
        }
    }, [draft, session]);

    return (
        <SafeAreaView style={{ flex: 1, padding: 24 }}>
            <StatusBar style="auto" />
            <Text style={{ fontSize: 22, fontWeight: '600', marginBottom: 12 }}>
                Banana v2 (mobile)
            </Text>
            <Text style={{ fontSize: 12, color: '#555', marginBottom: 16 }}>
                API base: {API_BASE_URL || '<unset>'}
            </Text>

            <View style={{ flexDirection: 'row', alignItems: 'center' }}>
                <BananaBadge count={0}> (today)</BananaBadge>
                <View style={{ width: 8 }} />
                <RipenessLabel value="yellow" />
            </View>

            <View
                style={{
                    marginTop: 16,
                    borderWidth: 1,
                    borderColor: '#e2e8f0',
                    borderRadius: 12,
                    backgroundColor: '#ffffff',
                    padding: 12,
                }}>
                <Text style={{ fontSize: 11, color: '#64748b', marginBottom: 8 }}>
                    Platform: {PLATFORM} | Session: {session?.id ?? (isBootstrapping ? 'initializing' : 'missing')}
                </Text>

                <ScrollView
                    style={{ maxHeight: 280, borderWidth: 1, borderColor: '#e2e8f0', borderRadius: 10, backgroundColor: '#f8fafc' }}
                    contentContainerStyle={{ padding: 10 }}>
                    {messages.map((message) => (
                        <ChatMessageBubble key={message.id} message={message} />
                    ))}
                    {messages.length === 0 && !isBootstrapping ? (
                        <Text style={{ fontSize: 12, color: '#64748b' }}>No chat messages yet.</Text>
                    ) : null}
                </ScrollView>

                <TextInput
                    style={{
                        marginTop: 10,
                        borderWidth: 1,
                        borderColor: '#cbd5e1',
                        borderRadius: 8,
                        paddingHorizontal: 10,
                        paddingVertical: 8,
                        fontSize: 14,
                    }}
                    value={draft}
                    onChangeText={setDraft}
                    editable={!isSending && !isBootstrapping && Boolean(session) && Boolean(API_BASE_URL)}
                    placeholder="Ask the banana assistant"
                />

                <Pressable
                    onPress={() => {
                        void sendMessage();
                    }}
                    disabled={
                        isSending ||
                        isBootstrapping ||
                        !session ||
                        !API_BASE_URL ||
                        draft.trim().length === 0
                    }
                    style={{
                        marginTop: 10,
                        borderRadius: 8,
                        backgroundColor:
                            isSending || isBootstrapping || !session || draft.trim().length === 0
                                ? '#cbd5e1'
                                : '#65a30d',
                        paddingVertical: 10,
                        alignItems: 'center',
                    }}>
                    <Text style={{ color: '#ffffff', fontWeight: '600' }}>
                        {isSending ? 'Sending...' : 'Send'}
                    </Text>
                </Pressable>

                {isBootstrapping ? (
                    <View style={{ marginTop: 8, flexDirection: 'row', alignItems: 'center' }}>
                        <ActivityIndicator size="small" color="#65a30d" />
                        <Text style={{ marginLeft: 6, fontSize: 12, color: '#64748b' }}>
                            Initializing chat session...
                        </Text>
                    </View>
                ) : null}

                {chatError ? (
                    <Text style={{ marginTop: 8, fontSize: 12, color: '#b91c1c' }}>{chatError}</Text>
                ) : null}
            </View>
        </SafeAreaView>
    );
}

registerRootComponent(App);
