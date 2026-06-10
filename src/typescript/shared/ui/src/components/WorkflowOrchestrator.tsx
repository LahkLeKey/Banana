import type { CSSProperties } from 'react';

export type WorkflowDepth = 1 | 2 | 3;

export type WorkflowOverlayModel = {
    readonly heading: string;
    readonly title: string;
    readonly summary: string;
    readonly detailA: string;
    readonly detailB: string;
};

export type WorkflowStepOption<TStep extends string> = {
    readonly id: TStep;
    readonly label: string;
};

type WorkflowOverlayStackProps = {
    readonly depth: WorkflowDepth;
    readonly model: WorkflowOverlayModel;
};

const stackFrameStyle: CSSProperties = {
    position: 'absolute',
    left: '50%',
    top: '52%',
    transform: 'translate(-50%, -50%)',
    width: 'min(560px, calc(100% - 48px))',
    height: 'min(300px, 56%)',
    pointerEvents: 'none',
};

export function WorkflowOverlayStack({ depth, model }: WorkflowOverlayStackProps) {
    return (
        <div style={stackFrameStyle}>
            {depth >= 1 ? (
                <div style={{
                    position: 'absolute',
                    inset: '14% 14% 14% 14%',
                    borderRadius: 14,
                    border: '1px solid rgba(125, 211, 252, 0.36)',
                    background: 'linear-gradient(150deg, rgba(8, 47, 73, 0.55), rgba(2, 8, 18, 0.82))',
                    boxShadow: '0 14px 28px rgba(2, 6, 23, 0.42)',
                    padding: 12,
                }}>
                    <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#7dd3fc', fontWeight: 700 }}>{model.heading}</div>
                    <div style={{ marginTop: 6, fontSize: 16, color: '#e0f2fe', fontWeight: 700 }}>{model.title}</div>
                    <div style={{ marginTop: 5, fontSize: 12, color: '#bae6fd' }}>{model.summary}</div>
                </div>
            ) : null}

            {depth >= 2 ? (
                <div style={{
                    position: 'absolute',
                    inset: '24% 8% 8% 22%',
                    borderRadius: 14,
                    border: '1px solid rgba(94, 234, 212, 0.38)',
                    background: 'linear-gradient(155deg, rgba(15, 118, 110, 0.52), rgba(2, 8, 18, 0.84))',
                    boxShadow: '0 14px 28px rgba(2, 6, 23, 0.45)',
                    padding: 12,
                }}>
                    <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#5eead4', fontWeight: 700 }}>Sub Overlay</div>
                    <div style={{ marginTop: 6, fontSize: 13, color: '#ccfbf1' }}>{model.detailA}</div>
                    <div style={{ marginTop: 4, fontSize: 12, color: '#99f6e4' }}>{model.detailB}</div>
                </div>
            ) : null}

            {depth >= 3 ? (
                <div style={{
                    position: 'absolute',
                    inset: '34% 24% 2% 36%',
                    borderRadius: 14,
                    border: '1px solid rgba(244, 114, 182, 0.42)',
                    background: 'linear-gradient(155deg, rgba(131, 24, 67, 0.55), rgba(2, 8, 18, 0.88))',
                    boxShadow: '0 14px 28px rgba(2, 6, 23, 0.5)',
                    padding: 10,
                }}>
                    <div style={{ fontSize: 10, textTransform: 'uppercase', letterSpacing: '0.08em', color: '#f9a8d4', fontWeight: 700 }}>Deep Overlay</div>
                    <div style={{ marginTop: 6, fontSize: 12, color: '#fbcfe8' }}>Workflow depth {depth} armed for commit.</div>
                </div>
            ) : null}
        </div>
    );
}

type WorkflowCenterToolbarProps<TStep extends string> = {
    readonly activeStep: TStep;
    readonly onStepChange: (step: TStep) => void;
    readonly depth: WorkflowDepth;
    readonly onDepthChange: (depth: WorkflowDepth) => void;
    readonly steps: WorkflowStepOption<TStep>[];
};

export function WorkflowCenterToolbar<TStep extends string>({
    activeStep,
    onStepChange,
    depth,
    onDepthChange,
    steps,
}: WorkflowCenterToolbarProps<TStep>) {
    return (
        <div style={{
            position: 'absolute',
            left: '50%',
            bottom: 14,
            transform: 'translateX(-50%)',
            borderRadius: 999,
            border: '1px solid rgba(148, 163, 184, 0.34)',
            background: 'linear-gradient(180deg, rgba(8, 20, 36, 0.92), rgba(2, 10, 22, 0.92))',
            boxShadow: '0 14px 26px rgba(2, 6, 23, 0.45)',
            padding: '6px 8px',
            display: 'flex',
            gap: 6,
            flexWrap: 'wrap',
            justifyContent: 'center',
            width: 'min(760px, calc(100% - 24px))',
        }}>
            {steps.map((step) => (
                <button
                    key={step.id}
                    type="button"
                    onClick={() => onStepChange(step.id)}
                    style={{
                        borderRadius: 999,
                        border: '1px solid rgba(125, 211, 252, 0.44)',
                        background: activeStep === step.id ? 'rgba(8, 47, 73, 0.92)' : 'rgba(15, 23, 42, 0.62)',
                        color: activeStep === step.id ? '#e0f2fe' : '#cbd5e1',
                        padding: '5px 10px',
                        fontSize: 11,
                        fontWeight: 700,
                        letterSpacing: '0.06em',
                        textTransform: 'uppercase',
                        cursor: 'pointer',
                    }}
                >
                    {step.label}
                </button>
            ))}

            {([1, 2, 3] as const).map((layer) => (
                <button
                    key={layer}
                    type="button"
                    onClick={() => onDepthChange(layer)}
                    style={{
                        borderRadius: 999,
                        border: '1px solid rgba(244, 114, 182, 0.42)',
                        background: depth === layer ? 'rgba(131, 24, 67, 0.86)' : 'rgba(30, 41, 59, 0.7)',
                        color: depth === layer ? '#fce7f3' : '#cbd5e1',
                        padding: '5px 10px',
                        fontSize: 11,
                        fontWeight: 700,
                        letterSpacing: '0.06em',
                        textTransform: 'uppercase',
                        cursor: 'pointer',
                    }}
                >
                    L{layer}
                </button>
            ))}
        </div>
    );
}
