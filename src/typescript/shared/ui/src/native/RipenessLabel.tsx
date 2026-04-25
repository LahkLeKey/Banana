import { Text } from 'react-native';

import type { Ripeness } from '../types';

const COLORS: Record<Ripeness, { backgroundColor: string; color: string }> = {
    green: { backgroundColor: '#bbf7d0', color: '#14532d' },
    yellow: { backgroundColor: '#fde68a', color: '#78350f' },
    spotted: { backgroundColor: '#fcd34d', color: '#78350f' },
    brown: { backgroundColor: '#78350f', color: '#fef3c7' },
};

export function RipenessLabel({ value }: { value: Ripeness }) {
    const color = COLORS[value];
    return (
        <Text
            style={{
                borderRadius: 6,
                overflow: 'hidden',
                backgroundColor: color.backgroundColor,
                color: color.color,
                fontSize: 12,
                paddingHorizontal: 6,
                paddingVertical: 2,
            }}>
            {value}
        </Text>
    );
}
