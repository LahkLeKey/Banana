import { Text, View } from 'react-native';

import type { ChatMessage } from '../types';

function bubbleColors(role: ChatMessage['role']): { backgroundColor: string; borderColor: string; textColor: string; align: 'flex-start' | 'flex-end' | 'center' } {
    if (role === 'user') {
        return {
            backgroundColor: '#fef3c7',
            borderColor: '#fcd34d',
            textColor: '#78350f',
            align: 'flex-end',
        };
    }

    if (role === 'assistant') {
        return {
            backgroundColor: '#ecfccb',
            borderColor: '#bef264',
            textColor: '#3f6212',
            align: 'flex-start',
        };
    }

    return {
        backgroundColor: '#f1f5f9',
        borderColor: '#cbd5e1',
        textColor: '#334155',
        align: 'center',
    };
}

export function ChatMessageBubble({ message }: { message: ChatMessage }) {
    const colors = bubbleColors(message.role);
    return (
        <View style={{ alignSelf: colors.align, maxWidth: '92%', marginBottom: 8 }}>
            <View
                style={{
                    borderRadius: 10,
                    borderWidth: 1,
                    borderColor: colors.borderColor,
                    backgroundColor: colors.backgroundColor,
                    paddingHorizontal: 10,
                    paddingVertical: 8,
                }}>
                <Text style={{ color: colors.textColor, fontSize: 14, lineHeight: 18 }}>
                    {message.content}
                </Text>
                <Text style={{ color: colors.textColor, fontSize: 10, marginTop: 6, textTransform: 'uppercase' }}>
                    {message.role} - {message.status}
                </Text>
            </View>
        </View>
    );
}