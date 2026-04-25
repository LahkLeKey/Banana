import type { ReactNode } from 'react';
import { Text, View } from 'react-native';

export function BananaBadge({ count, children }: { count: number; children?: ReactNode }) {
    return (
        <View
            style={{
                flexDirection: 'row',
                alignItems: 'center',
                borderRadius: 8,
                backgroundColor: '#fde68a',
                paddingHorizontal: 8,
                paddingVertical: 4,
                gap: 4,
            }}>
            <Text style={{ fontSize: 12, fontWeight: '600', color: '#78350f' }}>🍌 {count}</Text>
            {children ? <Text style={{ fontSize: 12, color: '#78350f' }}>{children}</Text> : null}
        </View>
    );
}
