// @banana/ui native annotation: React Native primitives only.
// Slice 027 -- canonical RN error wording surface.

import type { ReactNode } from 'react';
import { Text } from 'react-native';
import { tokens } from '../tokens';

export type ErrorTextProps = {
    children: ReactNode;
};

export function ErrorText({ children }: ErrorTextProps) {
    return (
        <Text
            testID="error-text"
            accessibilityRole="alert"
            style={{
                color: tokens.color.text.error,
                fontSize: tokens.font.size.xs,
            }}>
            {children}
        </Text>
    );
}
