import { useMemo } from 'react';

export type SparklineMarker = {
    readonly pointIndex: number;
    readonly color: string;
    readonly title: string;
};

export function markerColorForLogLevel(level: 'info' | 'success' | 'warn' | 'error'): string {
    if (level === 'success') {
        return '#34d399';
    }
    if (level === 'warn') {
        return '#fbbf24';
    }
    if (level === 'error') {
        return '#fb7185';
    }
    return '#38bdf8';
}

export function TelemetrySparkline({
    values,
    width = 220,
    height = 48,
    stroke = 'rgba(45, 212, 191, 0.9)',
    markers = [],
}: {
    readonly values: readonly number[];
    readonly width?: number;
    readonly height?: number;
    readonly stroke?: string;
    readonly markers?: readonly SparklineMarker[];
}) {
    const points = useMemo(() => {
        if (values.length < 2) {
            return null;
        }
        const min = Math.min(...values);
        const max = Math.max(...values);
        const range = max - min || 1;
        return values.map((value, index) => {
            const x = 6 + (index / (values.length - 1)) * (width - 12);
            const y = height - 6 - ((value - min) / range) * (height - 12);
            return { x, y };
        });
    }, [height, values, width]);

    if (!points) {
        return null;
    }

    const path = points
        .map((point, index) =>
            `${index === 0 ? 'M' : 'L'} ${point.x.toFixed(2)} ${point.y.toFixed(2)}`)
        .join(' ');

    return (
        <svg width={width} height={height} style={{ display: 'block' }}>
            <path d={path} fill="none" stroke={stroke} strokeWidth={1.4} />
            {markers.map((marker) => {
                const clampedIndex = Math.min(points.length - 1, Math.max(0, marker.pointIndex));
                const point = points[clampedIndex];
                return (
                    <g key={`${marker.title}-${clampedIndex}`}>
                        <line
                            x1={point.x}
                            y1={height - 4}
                            x2={point.x}
                            y2={height - 12}
                            stroke={marker.color}
                            strokeWidth={1}
                            opacity={0.9}
                        />
                        <circle
                            cx={point.x}
                            cy={point.y}
                            r={2}
                            fill={marker.color}
                            stroke="rgba(2, 10, 20, 0.92)"
                            strokeWidth={0.9}
                        >
                            <title>{marker.title}</title>
                        </circle>
                    </g>
                );
            })}
        </svg>
    );
}
