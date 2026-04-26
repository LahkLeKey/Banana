// @banana/ui native annotation: React Native primitives only.
// Slice 027 -- canonical RN retry button mirroring the web variant.

import { Pressable, Text } from 'react-native';
import { tokens } from '../tokens';

export const RETRY_BUTTON_COPY = 'Try again';

export type RetryButtonProps = {
    label?: string;
    onPress: () => void;
    disabled?: boolean;
};

export function RetryButton({ label = RETRY_BUTTON_COPY, onPress, disabled }: RetryButtonProps) {
    return (
        <Pressable
            testID="retry-button"
            onPress={onPress}
            disabled={disabled}
            style={{
                backgroundColor: tokens.color.escalation.bg,
                borderColor: tokens.color.escalation.accent,
                borderWidth: 1,
                borderRadius: tokens.radius.md,
                paddingHorizontal: tokens.space[3],
                paddingVertical: tokens.space[2],
                opacity: disabled ? 0.5 : 1,
                alignItems: 'center',
            }}>
            <Text
                style={{
                    color: tokens.color.escalation.fg,
                    fontSize: tokens.font.size.sm,
                    fontWeight: tokens.font.weight.semibold,
                }}>
                {label}
            </Text>
        </Pressable>
    );
}
