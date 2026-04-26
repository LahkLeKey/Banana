// Slice 027 -- ErrorText coverage (web).
// @ts-nocheck
import { afterEach, beforeEach, describe, expect, test } from 'bun:test';
import { GlobalRegistrator } from '@happy-dom/global-registrator';
import { act } from 'react';
import { createRoot, type Root } from 'react-dom/client';

import { ErrorText } from './ErrorText';
import { tokens } from '../tokens';

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

describe('ErrorText', () => {
    test('renders children inside an error-toned alert paragraph', () => {
        act(() => { root.render(<ErrorText>Something went wrong</ErrorText>); });
        const node = container.querySelector('[data-testid="error-text"]') as HTMLParagraphElement;
        expect(node).not.toBeNull();
        expect(node.tagName).toBe('P');
        expect(node.textContent).toBe('Something went wrong');
        expect(node.getAttribute('role')).toBe('alert');
    });

    test('uses the text-error token color', () => {
        act(() => { root.render(<ErrorText>err</ErrorText>); });
        const node = container.querySelector('[data-testid="error-text"]') as HTMLParagraphElement;
        // happy-dom returns inline style as set; rgb conversion is not guaranteed.
        expect(node.style.color.toLowerCase()).toContain(tokens.color.text.error.toLowerCase());
    });
});
