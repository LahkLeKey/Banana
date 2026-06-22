import { RadarMini } from './RadarMini';
import { MetricBar, MetricPill, SectionLabel } from './TelemetryPrimitives';

export type TelemetryHealthBar = {
    readonly label: string;
    readonly value: number;
    readonly color: string;
};

export function TelemetryHealthCard({
    title,
    titleColor,
    score,
    scoreTone,
    radarAxes,
    radarStroke,
    radarFill,
    bars,
    summary,
    helper,
    showRadar,
    mobile,
    border,
}: {
    readonly title: string;
    readonly titleColor: string;
    readonly score: number;
    readonly scoreTone: 'default' | 'good' | 'bad';
    readonly radarAxes: readonly { label: string; value: number }[];
    readonly radarStroke: string;
    readonly radarFill: string;
    readonly bars: readonly TelemetryHealthBar[];
    readonly summary: string;
    readonly helper: string;
    readonly showRadar: boolean;
    readonly mobile: boolean;
    readonly border: string;
}) {
    return (
        <div
            style={{
                borderRadius: 12,
                border,
                background: 'rgba(2, 10, 20, 0.38)',
                backdropFilter: 'blur(4px)',
                padding: 6,
                display: 'grid',
                gap: 5,
            }}
        >
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <SectionLabel color={titleColor}>{title}</SectionLabel>
                <MetricPill label="score" value={`${score}%`} tone={scoreTone} />
            </div>
            <div style={{ display: 'grid', gridTemplateColumns: mobile ? '1fr' : '132px 1fr', gap: 8, alignItems: 'center' }}>
                {showRadar ? (
                    <RadarMini axes={radarAxes} size={128} stroke={radarStroke} fill={radarFill} />
                ) : null}
                <div style={{ display: 'grid', gap: 5 }}>
                    {bars.map((bar) => (
                        <MetricBar key={bar.label} label={bar.label} value={bar.value} color={bar.color} />
                    ))}
                </div>
            </div>
            <div style={{ display: 'grid', gap: 3 }}>
                <div style={{ fontSize: 10, color: '#94a3b8' }}>{summary}</div>
                <div style={{ fontSize: 9, color: '#64748b' }}>{helper}</div>
            </div>
        </div>
    );
}
