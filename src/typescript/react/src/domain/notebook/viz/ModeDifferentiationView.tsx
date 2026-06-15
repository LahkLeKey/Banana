import { useMemo, useState } from 'react';

/** Score emitted for one token under one k3h4 assignment mode. */
export type TokenScore = {
    readonly tokenId: string;
    readonly clusterId: number;
    readonly score: number;
};

/** Props for the cross-mode notebook comparison panel. */
export type ModeDifferentiationViewProps = {
    readonly multiplicativeScores: readonly TokenScore[];
    readonly powerScores: readonly TokenScore[];
    readonly title?: string;
    readonly comparisonMode?: 'observed' | 'synthetic';
};

type TokenScorePair = {
    readonly tokenId: string;
    readonly multiplicative: TokenScore | undefined;
    readonly power: TokenScore | undefined;
    readonly divergent: boolean;
};

/* Aligns tokens across the two modes and flags cluster-assignment divergence. */
function buildTokenPairs(
    multiplicativeScores: readonly TokenScore[],
    powerScores: readonly TokenScore[],
): TokenScorePair[] {
    const powerByToken = new Map(powerScores.map((entry) => [entry.tokenId, entry]));
    const pairs: TokenScorePair[] = [];

    for (const multiplicative of multiplicativeScores) {
        const power = powerByToken.get(multiplicative.tokenId);
        pairs.push({
            tokenId: multiplicative.tokenId,
            multiplicative,
            power,
            divergent:
                power !== undefined && power.clusterId !== multiplicative.clusterId,
        });
    }

    return pairs;
}

function computeMaxScore(
    multiplicativeScores: readonly TokenScore[],
    powerScores: readonly TokenScore[],
): number {
    const combined = [...multiplicativeScores, ...powerScores];
    const maxScore = combined.reduce((max, entry) => Math.max(max, entry.score), 0);
    return maxScore > 0 ? maxScore : 1;
}

function panelLabel(mode: 'multiplicative' | 'power'): string {
    return mode === 'multiplicative' ? 'Multiplicative' : 'Power';
}

function scoreWidth(score: number, maxScore: number): string {
    return `${Math.max(6, Math.round((score / maxScore) * 100))}%`;
}

function tokenKey(mode: 'multiplicative' | 'power', tokenId: string): string {
    return `${mode}:${tokenId}`;
}

/**
 * Renders side-by-side token score distributions for multiplicative and power
 * mode so notebook users can spot divergence or degeneracy in one glance.
 */
export function ModeDifferentiationView({
    multiplicativeScores,
    powerScores,
    title = 'Mode Differentiation',
    comparisonMode = 'observed',
}: ModeDifferentiationViewProps) {
    const [selectedTokenId, setSelectedTokenId] = useState<string | null>(null);
    const tokenPairs = useMemo(
        () => buildTokenPairs(multiplicativeScores, powerScores),
        [multiplicativeScores, powerScores]);
    const maxScore = useMemo(
        () => computeMaxScore(multiplicativeScores, powerScores),
        [multiplicativeScores, powerScores]);
    const isDegenerate = comparisonMode === 'observed' && tokenPairs.length > 0 && tokenPairs.every((pair) => {
        const multiplicative = pair.multiplicative;
        const power = pair.power;
        if (!multiplicative || !power) {
            return false;
        }

        return multiplicative.clusterId === power.clusterId &&
            Math.abs(multiplicative.score - power.score) < 0.0001;
    });

    return (
        <section className="flex flex-col gap-4 rounded-xl border border-slate-200 bg-white p-4 shadow-sm">
            <header className="flex flex-col gap-1">
                <h2 className="text-base font-semibold text-slate-950">{title}</h2>
                <p className="text-sm text-slate-500">
                    Side-by-side score distributions for the same token order and x-axis scale.
                </p>
                {comparisonMode === 'synthetic' ? (
                    <div
                        role="status"
                        className="rounded-md border border-sky-200 bg-sky-50 px-3 py-2 text-sm text-sky-900"
                    >
                        single-mode snapshot - cross-mode separation unavailable until both mode outputs are exported
                    </div>
                ) : null}
                {isDegenerate ? (
                    <div
                        role="status"
                        className="rounded-md border border-amber-200 bg-amber-50 px-3 py-2 text-sm text-amber-900"
                    >
                        degenerate result - mode separation not observed
                    </div>
                ) : null}
            </header>

            <div className="grid gap-4 md:grid-cols-2">
                {([
                    { mode: 'multiplicative' as const, scores: multiplicativeScores, accent: 'bg-emerald-500' },
                    { mode: 'power' as const, scores: powerScores, accent: 'bg-amber-500' },
                ]).map((panel) => (
                    <div
                        key={panel.mode}
                        className="rounded-lg border border-slate-200 bg-slate-50 p-3"
                        data-testid={`mode-panel-${panel.mode}`}
                    >
                        <div className="mb-3 flex items-center justify-between gap-3">
                            <h3 className="text-sm font-semibold uppercase tracking-wide text-slate-700">
                                {panelLabel(panel.mode)}
                            </h3>
                            <span className="text-xs text-slate-500">
                                Same token order, same x-axis scale
                            </span>
                        </div>

                        <div className="flex flex-col gap-2">
                            {panel.scores.map((entry) => {
                                const selected = selectedTokenId === entry.tokenId;
                                const matchingPair = tokenPairs.find((pair) => pair.tokenId === entry.tokenId);
                                const divergent = Boolean(matchingPair?.divergent);
                                return (
                                    <button
                                        key={tokenKey(panel.mode, entry.tokenId)}
                                        type="button"
                                        onClick={() => setSelectedTokenId(entry.tokenId)}
                                        aria-pressed={selected}
                                        data-selected={selected ? 'true' : 'false'}
                                        data-divergent={divergent ? 'true' : 'false'}
                                        className={[
                                            'flex w-full items-center gap-3 rounded-md border px-3 py-2 text-left transition',
                                            selected ? 'border-slate-950 bg-white' : 'border-slate-200 bg-white',
                                            divergent ? 'ring-1 ring-rose-400' : '',
                                        ].join(' ')}
                                    >
                                        <div className="min-w-20 text-sm font-medium text-slate-900">
                                            {entry.tokenId}
                                        </div>
                                        <div className="flex-1">
                                            <div className="h-2 overflow-hidden rounded-full bg-slate-100">
                                                <div
                                                    className={`h-full ${panel.accent}`}
                                                    style={{ width: scoreWidth(entry.score, maxScore) }}
                                                />
                                            </div>
                                        </div>
                                        <div className="flex min-w-24 flex-col items-end text-xs text-slate-500">
                                            <span>score {entry.score.toFixed(2)}</span>
                                            <span>cluster {entry.clusterId}</span>
                                        </div>
                                    </button>
                                );
                            })}
                        </div>
                    </div>
                ))}
            </div>

            <footer className="flex flex-wrap gap-2 text-xs text-slate-500">
                {tokenPairs.map((pair) => {
                    const isSelected = selectedTokenId === pair.tokenId;
                    return (
                        <span
                            key={pair.tokenId}
                            className={[
                                'rounded-full border px-2 py-1',
                                isSelected ? 'border-slate-950 bg-slate-950 text-white' : 'border-slate-200 bg-white',
                                pair.divergent ? 'border-rose-400' : '',
                            ].join(' ')}
                        >
                            {pair.tokenId}
                        </span>
                    );
                })}
            </footer>
        </section>
    );
}
