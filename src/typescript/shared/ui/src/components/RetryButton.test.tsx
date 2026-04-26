// Slice 027 -- RetryButton coverage (web).
// @ts-nocheck
import { afterEach, beforeEach, describe, expect, mock, test } from 'bun:test';
import { GlobalRegistrator } from '@happy-dom/global-registrator';
import { act } from 'react';
import { createRoot, type Root } from 'react-dom/client';

import { RetryButton, RETRY_BUTTON_COPY } from './RetryButton';

(globalThis as unknown as { IS_REACT_ACT_ENVIRONMENT: boolean }).IS_REACT_ACT_ENVIRONMENT = true;

let container: HTMLDivElement;
let root: Root;

beforeEach(() => {
    if (!(globalThis as unknown as { document?: Document }).document) {
        GlobalRegistrator.register();
    }
    container = document.createElement('div');
    document.body.appendChild(container);
    root = createRoot(container);
});

afterEach(() => {
    act(() => { root.unmount(); });
    container.remove();
});

describe('RetryButton', () => {
    test('renders default copy', () => {
        act(() => { root.render(<RetryButton onClick={() => { }} />); });
        const btn = container.querySelector('[data-testid="retry-button"]') as HTMLButtonElement;
        expect(btn).not.toBeNull();
        expect(btn.textContent).toBe(RETRY_BUTTON_COPY);
        expect(btn.textContent).toBe('Try again');
    });

    test('honors label override', () => {
        act(() => { root.render(<RetryButton onClick={() => { }} label="Retry capture" />); });
        const btn = container.querySelector('[data-testid="retry-button"]') as HTMLButtonElement;
        expect(btn.textContent).toBe('Retry capture');
    });

    test('fires onClick when pressed', () => {
        const onClick = mock(() => { });
        act(() => { root.render(<RetryButton onClick={onClick} />); });
        const btn = container.querySelector('[data-testid="retry-button"]') as HTMLButtonElement;
        act(() => { btn.click(); });
        expect(onClick).toHaveBeenCalledTimes(1);
    });

    test('respects disabled prop', () => {
        const onClick = mock(() => { });
        act(() => { root.render(<RetryButton onClick={onClick} disabled />); });
        const btn = container.querySelector('[data-testid="retry-button"]') as HTMLButtonElement;
        expect(btn.disabled).toBe(true);
        act(() => { btn.click(); });
        expect(onClick).toHaveBeenCalledTimes(0);
    });
});
