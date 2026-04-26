// @banana/ui native annotation: React Native primitives only.
// Slice 027 -- token-driven.
import { Text, View } from 'react-native';
import { tokens } from '../tokens';
import type { ChatMessage } from '../types';

type ChatRole = ChatMessage['role'];

function bubbleTone(role: ChatRole): { bg: string; fg: string; align: 'flex-start' | 'flex-end' | 'center' } {
    if (role === 'user') {
        return { bg: tokens.color.banana.bg, fg: tokens.color.banana.fg, align: 'flex-end' };
    }
    if (role === 'assistant') {
        return { bg: tokens.color.surface.muted, fg: tokens.color.text.default, align: 'flex-start' };
    }
    return { bg: tokens.color.surface.muted, fg: tokens.color.text.muted, align: 'center' };
}

export function ChatMessageBubble({ message }: { message: ChatMessage }) {
    const tone = bubbleTone(message.role);
    return (
        <View style={{ alignSelf: tone.align, maxWidth: '92%', marginBottom: tokens.space[2] }}>
            <View
                style={{
                    borderRadius: tokens.radius.md,
                    backgroundColor: tone.bg,
                    paddingHorizontal: tokens.space[3],
                    paddingVertical: tokens.space[2],
                }}>
                <Text style={{ color: tone.fg, fontSize: tokens.font.size.sm, lineHeight: 18 }}>
                    {message.content}
                </Text>
                <Text
                    style={{
                        color: tone.fg,
                        fontSize: 10,
                        marginTop: tokens.space[1],
                        textTransform: 'uppercase',
                        opacity: 0.7,
                    }}>
                    {message.role} - {message.status}
                </Text>
            </View>
        </View>
    );
}
