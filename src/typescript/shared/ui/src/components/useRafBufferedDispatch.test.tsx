import { afterEach, beforeEach, describe, expect, mock, test } from 'bun:test';
import { GlobalRegistrator } from '@happy-dom/global-registrator';
import { act } from 'react';
import { createRoot, type Root } from 'react-dom/client';

import { type RafBufferedDispatch, useRafBufferedDispatch } from './useRafBufferedDispatch';

let container: HTMLDivElement;
let root: Root;
let api: RafBufferedDispatch<number> | null = null;
let originalRaf: typeof window.requestAnimationFrame;
let originalCancelRaf: typeof window.cancelAnimationFrame;

function Harness({ onDispatch }: { onDispatch?: (payload: number) => void }) {
    api = useRafBufferedDispatch(onDispatch);
    return null;
}

beforeEach(() => {
    if (!(globalThis as unknown as { document?: Document }).document) {
        GlobalRegistrator.register();
    }

    api = null;
    originalRaf = window.requestAnimationFrame;
    originalCancelRaf = window.cancelAnimationFrame;

    container = document.createElement('div');
    document.body.appendChild(container);
    root = createRoot(container);
});

afterEach(() => {
    act(() => {
        root.unmount();
    });
    container.remove();
    window.requestAnimationFrame = originalRaf;
    window.cancelAnimationFrame = originalCancelRaf;
});

describe('useRafBufferedDispatch', () => {
    test('flush(true) commits the pending payload once', () => {
        const onDispatch = mock(() => { });
        const rafCallbacks: FrameRequestCallback[] = [];

        window.requestAnimationFrame = ((cb: FrameRequestCallback) => {
            rafCallbacks.push(cb);
            return rafCallbacks.length;
        }) as typeof window.requestAnimationFrame;
        window.cancelAnimationFrame = (() => { }) as typeof window.cancelAnimationFrame;

        act(() => {
            root.render(<Harness onDispatch={onDispatch} />);
        });

        act(() => {
            api!.schedule(7);
            api!.schedule(11);
            api!.flush(true);
        });

        expect(onDispatch).toHaveBeenCalledTimes(1);
        expect(onDispatch).toHaveBeenCalledWith(11);

        // Deferred RAF callback should no-op after flush cleared pending state.
        act(() => {
            for (const cb of rafCallbacks) {
                cb(0);
            }
        });

        expect(onDispatch).toHaveBeenCalledTimes(1);
    });

    test('cancels pending RAF during unmount cleanup', () => {
        const onDispatch = mock(() => { });
        const cancel = mock(() => { });

        window.requestAnimationFrame = ((_cb: FrameRequestCallback) => 42) as typeof window.requestAnimationFrame;
        window.cancelAnimationFrame = cancel as unknown as typeof window.cancelAnimationFrame;

        act(() => {
            root.render(<Harness onDispatch={onDispatch} />);
        });

        act(() => {
            api!.schedule(23);
        });

        act(() => {
            root.unmount();
        });

        expect(cancel).toHaveBeenCalledTimes(1);
        expect(cancel).toHaveBeenCalledWith(42);
        expect(onDispatch).toHaveBeenCalledTimes(0);
    });
});
