import '../../../test-setup';

import { describe, expect, test } from 'bun:test';
import { render, screen } from '@testing-library/react';
import userEvent from '@testing-library/user-event';

import { ModeDifferentiationView, type TokenScore } from './ModeDifferentiationView';

const MULTIPLICATIVE_SCORES: readonly TokenScore[] = [
    { tokenId: 'tok-1', clusterId: 0, score: 0.42 },
    { tokenId: 'tok-2', clusterId: 1, score: 0.84 },
    { tokenId: 'tok-3', clusterId: 1, score: 0.63 },
];

const POWER_SCORES: readonly TokenScore[] = [
    { tokenId: 'tok-1', clusterId: 0, score: 0.40 },
    { tokenId: 'tok-2', clusterId: 2, score: 0.77 },
    { tokenId: 'tok-3', clusterId: 1, score: 0.63 },
];

describe('ModeDifferentiationView', () => {
    test('renders both mode panels with token scores', () => {
        render(
            <ModeDifferentiationView
                multiplicativeScores={MULTIPLICATIVE_SCORES}
                powerScores={POWER_SCORES}
            />,
        );

        expect(screen.getByText('Mode Differentiation')).toBeTruthy();
        expect(screen.getByText('Multiplicative')).toBeTruthy();
        expect(screen.getByText('Power')).toBeTruthy();
        expect(screen.getAllByRole('button', { name: /tok-1/i })).toHaveLength(2);
        expect(screen.getAllByRole('button', { name: /tok-2/i })).toHaveLength(2);
        expect(screen.getAllByRole('button', { name: /tok-3/i })).toHaveLength(2);
    });

    test('highlights divergent tokens in both panels', () => {
        render(
            <ModeDifferentiationView
                multiplicativeScores={MULTIPLICATIVE_SCORES}
                powerScores={POWER_SCORES}
            />,
        );

        const multiplicativeTok2 = screen.getAllByRole('button', { name: /tok-2/i })[0] as HTMLButtonElement;
        const powerTok2 = screen.getAllByRole('button', { name: /tok-2/i })[1] as HTMLButtonElement;

        expect(multiplicativeTok2.dataset.divergent).toBe('true');
        expect(powerTok2.dataset.divergent).toBe('true');
    });

    test('selection is shared across panels', async () => {
        const user = userEvent.setup();
        render(
            <ModeDifferentiationView
                multiplicativeScores={MULTIPLICATIVE_SCORES}
                powerScores={POWER_SCORES}
            />,
        );

        const powerTok2 = screen.getAllByRole('button', { name: /tok-2/i })[1] as HTMLButtonElement;
        await user.click(powerTok2);

        const multiplicativeTok2 = screen.getAllByRole('button', { name: /tok-2/i })[0] as HTMLButtonElement;
        expect(multiplicativeTok2.dataset.selected).toBe('true');
        expect(powerTok2.dataset.selected).toBe('true');
    });

    test('annotates degenerate results', () => {
        const degenerate = [
            { tokenId: 'tok-a', clusterId: 0, score: 0.5 },
            { tokenId: 'tok-b', clusterId: 1, score: 0.75 },
        ] as const;

        render(
            <ModeDifferentiationView
                multiplicativeScores={degenerate}
                powerScores={degenerate}
            />,
        );

        expect(
            screen.getByText('degenerate result - mode separation not observed'))
            .toBeTruthy();
    });
});
