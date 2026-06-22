import { useMemo } from 'react';

type RadarAxis = {
    readonly label: string;
    readonly value: number;
};

type RadarMiniProps = {
    readonly axes: readonly RadarAxis[];
    readonly size?: number;
    readonly stroke?: string;
    readonly fill?: string;
};

function clampUnit(value: number): number {
    if (!Number.isFinite(value)) {
        return 0;
    }
    return Math.min(1, Math.max(0, value));
}

export function RadarMini({
    axes,
    size = 136,
    stroke = '#22d3ee',
    fill = 'rgba(34, 211, 238, 0.24)',
}: RadarMiniProps) {
    const geometry = useMemo(() => {
        const safeAxes = axes.length >= 3 ? axes : [
            ...axes,
            { label: 'pad-a', value: 0 },
            { label: 'pad-b', value: 0 },
            { label: 'pad-c', value: 0 },
        ].slice(0, 3);

        const count = safeAxes.length;
        const center = size / 2;
        const maxRadius = size * 0.34;
        const ringRatios = [0.25, 0.5, 0.75, 1];

        const axisPoints = safeAxes.map((axis, index) => {
            const angle = -Math.PI / 2 + (index / count) * (Math.PI * 2);
            const x = center + Math.cos(angle) * maxRadius;
            const y = center + Math.sin(angle) * maxRadius;
            const valueRadius = maxRadius * clampUnit(axis.value);
            const vx = center + Math.cos(angle) * valueRadius;
            const vy = center + Math.sin(angle) * valueRadius;

            return { axis, x, y, vx, vy, angle };
        });

        const rings = ringRatios.map((ratio) => {
            return axisPoints
                .map((point, index) => {
                    const x = center + Math.cos(point.angle) * maxRadius * ratio;
                    const y = center + Math.sin(point.angle) * maxRadius * ratio;
                    return `${index === 0 ? 'M' : 'L'} ${x.toFixed(2)} ${y.toFixed(2)}`;
                })
                .join(' ') + ' Z';
        });

        const valuePath = axisPoints
            .map((point, index) =>
                `${index === 0 ? 'M' : 'L'} ${point.vx.toFixed(2)} ${point.vy.toFixed(2)}`)
            .join(' ') + ' Z';

        return { center, rings, axisPoints, valuePath };
    }, [axes, size]);

    return (
        <svg width={size} height={size} style={{ display: 'block', overflow: 'visible' }}>
            {geometry.rings.map((path, index) => (
                <path
                    key={index}
                    d={path}
                    fill="none"
                    stroke="rgba(148, 163, 184, 0.22)"
                    strokeWidth={index === geometry.rings.length - 1 ? 1 : 0.8}
                />
            ))}

            {geometry.axisPoints.map((point, index) => (
                <g key={index}>
                    <line
                        x1={geometry.center}
                        y1={geometry.center}
                        x2={point.x}
                        y2={point.y}
                        stroke="rgba(148, 163, 184, 0.22)"
                        strokeWidth={0.8}
                    />
                    <text
                        x={geometry.center + Math.cos(point.angle) * (size * 0.44)}
                        y={geometry.center + Math.sin(point.angle) * (size * 0.44)}
                        fill="#64748b"
                        fontSize={8}
                        textAnchor="middle"
                        dominantBaseline="middle"
                    >
                        {point.axis.label}
                    </text>
                </g>
            ))}

            <path d={geometry.valuePath} fill={fill} stroke={stroke} strokeWidth={1.6} />

            {geometry.axisPoints.map((point, index) => (
                <circle
                    key={`v-${index}`}
                    cx={point.vx}
                    cy={point.vy}
                    r={2.2}
                    fill={stroke}
                    stroke="#0b1220"
                    strokeWidth={1}
                />
            ))}
        </svg>
    );
}
