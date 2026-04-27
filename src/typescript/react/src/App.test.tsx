// @ts-nocheck -- bun:test types not in app tsconfig; behavior asserted via DOM.
//
// Slice 023 -- React App customer-facing verdict surface.
// Validates the five baseline copy strings, the escalation cue + panel
// expansion, the retry affordance preserves the last submitted draft,
// and that editing the input clears the retry button.

import { afterEach, beforeEach, describe, expect, test } from 'bun:test';
import { act, cleanup, fireEvent, render, screen } from '@testing-library/react';

import { App } from './App';
import {
    didLocationChange,
    getLocationFingerprint,
} from './lib/__tests__/ensemble-submit-test-helpers';

let originalFetch: typeof fetch;

beforeEach(() => {
    // Inject API base via the Electron bridge fallback so we don't depend
    // on Vite's import.meta.env (Bun does not populate it during tests).
    (globalThis as any).window.banana = { apiBaseUrl: 'http://api.example', platform: '' };
    originalFetch = globalThis.fetch;
});

afterEach(() => {
    cleanup();
    globalThis.fetch = originalFetch;
    delete (globalThis as any).window.banana;
});

function makeFetchMock(handler: (input: RequestInfo | URL, init?: RequestInit) => Response): typeof fetch {
    return (async (input: RequestInfo | URL, init?: RequestInit) => handler(input, init)) as typeof fetch;
}

function jsonResponse(body: unknown, status = 200): Response {
    return new Response(JSON.stringify(body), {
        status,
        headers: { 'content-type': 'application/json' },
    });
}

async function flush() {
    for (let i = 0; i < 5; i += 1) {
        await act(async () => { await Promise.resolve(); });
    }
}

function defaultBootstrap(input: RequestInfo | URL): Response {
    const url = String(input);
    if (url.endsWith('/banana?purchases=3&multiplier=2')) {
        return jsonResponse({ banana: 0 });
    }
    if (url.endsWith('/chat/sessions')) {
        return jsonResponse({
            session: { id: 'chat_1', platform: 'web', created_at: 'x', updated_at: 'x', message_count: 1 },
            welcome_message: { id: 'm0', session_id: 'chat_1', role: 'assistant', content: 'hi', created_at: 'x', status: 'complete' },
        }, 201);
    }
    return jsonResponse({ error: { message: 'unexpected ' + url } }, 500);
}

async function submitEnsemble(text: string) {
    const textarea = screen.getByPlaceholderText('Describe a possible banana') as HTMLTextAreaElement;
    await act(async () => {
        fireEvent.change(textarea, { target: { value: text } });
    });
    const submitBtn = screen.getByRole('button', { name: /Predict ensemble/ }) as HTMLButtonElement;
    await act(async () => {
        fireEvent.click(submitBtn);
    });
    await flush();
}

async function keyboardSubmitEnsemble(text: string) {
    const textarea = screen.getByPlaceholderText('Describe a possible banana') as HTMLTextAreaElement;
    await act(async () => {
        fireEvent.change(textarea, { target: { value: text } });
    });
    await act(async () => {
        fireEvent.keyDown(textarea, { key: 'Enter', ctrlKey: true });
    });
    await flush();
}

describe('App ensemble verdict surface (slice 023)', () => {
    test('enables chat input after healthy bootstrap', async () => {
        globalThis.fetch = makeFetchMock(defaultBootstrap);
        await act(async () => { render(<App />); });
        await flush();

        const chatInput = screen.getByPlaceholderText('Ask the banana assistant') as HTMLInputElement;
        expect(chatInput.hasAttribute('disabled')).toBe(false);
        expect(screen.getByText('session: chat_1')).not.toBeNull();
        expect(screen.queryByTestId('chat-bootstrap-retry')).toBeNull();
    });

    test('shows bootstrap remediation and recovers after retry', async () => {
        let bootstrapAttempts = 0;
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/chat/sessions')) {
                bootstrapAttempts += 1;
                if (bootstrapAttempts === 1) {
                    throw new Error('Failed to fetch');
                }
                return jsonResponse({
                    session: { id: 'chat_1', platform: 'web', created_at: 'x', updated_at: 'x', message_count: 1 },
                    welcome_message: { id: 'm0', session_id: 'chat_1', role: 'assistant', content: 'hi', created_at: 'x', status: 'complete' },
                }, 201);
            }
            return defaultBootstrap(input);
        });

        await act(async () => { render(<App />); });
        await flush();

        expect(screen.getByText('Chat bootstrap transport failure: the chat runtime endpoint could not be reached.')).not.toBeNull();
        expect(screen.getByTestId('chat-bootstrap-remediation').textContent)
            .toContain('Ensure the runtime profile is healthy and the apps profile is up');

        await act(async () => {
            fireEvent.click(screen.getByTestId('chat-bootstrap-retry'));
        });
        await flush();

        expect(bootstrapAttempts).toBe(2);
        expect(screen.getByText('session: chat_1')).not.toBeNull();
        expect(screen.queryByTestId('chat-bootstrap-remediation')).toBeNull();
    });

    test('renders empty-state copy before any submit', async () => {
        globalThis.fetch = makeFetchMock(defaultBootstrap);
        await act(async () => { render(<App />); });
        await flush();

        expect(screen.getByTestId('ensemble-verdict-empty').textContent).toBe('Send a sample to get a verdict.');
    });

    test('click submit keeps location stable and renders verdict', async () => {
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                return jsonResponse({
                    verdict: { label: 'banana', score: 0.95, did_escalate: false, calibration_magnitude: 0.7, status: 'ok' },
                    embedding: null,
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();

        const before = getLocationFingerprint();
        await submitEnsemble('click path sample');
        const after = getLocationFingerprint();

        expect(didLocationChange(before, after)).toBe(false);
        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toBe('Banana.');
    });

    test('keyboard submit keeps location stable and renders verdict', async () => {
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                return jsonResponse({
                    verdict: { label: 'not_banana', score: 0.95, did_escalate: false, calibration_magnitude: 0.2, status: 'ok' },
                    embedding: null,
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();

        const before = getLocationFingerprint();
        await keyboardSubmitEnsemble('keyboard path sample');
        const after = getLocationFingerprint();

        expect(didLocationChange(before, after)).toBe(false);
        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toBe('Not a banana.');
    });

    test('renders confident banana copy on cheap-path verdict', async () => {
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                return jsonResponse({
                    verdict: { label: 'banana', score: 0.95, did_escalate: false, calibration_magnitude: 0.7, status: 'ok' },
                    embedding: null,
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();
        await submitEnsemble('ripe banana');

        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toBe('Banana.');
        expect(screen.queryByTestId('escalation-panel')).toBeNull();
    });

    test('renders escalated banana copy + EscalationPanel trigger', async () => {
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                return jsonResponse({
                    verdict: { label: 'banana', score: 0.83, did_escalate: true, calibration_magnitude: 0.6, status: 'ok' },
                    embedding: [0.42, -0.11, 0.07, 0.83],
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();
        await submitEnsemble('yellow fruit on the counter maybe');

        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toContain('Banana \u2014 needs a closer look.');
        expect(screen.getByTestId('escalation-panel-trigger')).not.toBeNull();
    });

    test('renders confident not-banana copy', async () => {
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                return jsonResponse({
                    verdict: { label: 'not_banana', score: 0.95, did_escalate: false, calibration_magnitude: 0.1, status: 'ok' },
                    embedding: null,
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();
        await submitEnsemble('engine oil');

        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toBe('Not a banana.');
    });

    test('renders escalated not-banana copy', async () => {
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                return jsonResponse({
                    verdict: { label: 'not_banana', score: 0.55, did_escalate: true, calibration_magnitude: 0.4, status: 'ok' },
                    embedding: [0.1, 0.5, 0.0, 0.4],
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();
        await submitEnsemble('yellow plastic toy shaped like a banana');

        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toContain('Not a banana \u2014 needs a closer look.');
    });

    test('preserves baseline verdict and escalation cues under guarded submit', async () => {
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                return jsonResponse({
                    verdict: { label: 'banana', score: 0.81, did_escalate: true, calibration_magnitude: 0.62, status: 'ok' },
                    embedding: [0.31, 0.12, 0.27, 0.81],
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();

        const before = getLocationFingerprint();
        await submitEnsemble('guarded baseline sample');
        const after = getLocationFingerprint();

        expect(didLocationChange(before, after)).toBe(false);
        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toContain('Banana \u2014 needs a closer look.');
        expect(screen.getByTestId('banana-badge-ensemble')).not.toBeNull();
        expect(screen.getByTestId('escalation-panel-trigger')).not.toBeNull();
    });

    test('shows retry button after error and reuses last submitted draft', async () => {
        let lastBody: unknown = null;
        let attempts = 0;
        globalThis.fetch = makeFetchMock((input, init) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                attempts += 1;
                if (init?.body) lastBody = JSON.parse(String(init.body));
                if (attempts === 1) {
                    return jsonResponse({ error: { message: 'banana service down' } }, 503);
                }
                return jsonResponse({
                    verdict: { label: 'banana', score: 0.92, did_escalate: false, calibration_magnitude: 0.5, status: 'ok' },
                    embedding: null,
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();

        await submitEnsemble('ambiguous sample');
        expect(screen.getByTestId('ensemble-error').textContent).toBe('Banana service is having a moment. Try again.');
        const retry = screen.getByTestId('ensemble-retry');
        expect(retry.textContent).toBe('Try again');

        await act(async () => {
            fireEvent.click(retry);
        });
        await flush();

        expect(attempts).toBe(2);
        const inner = JSON.parse((lastBody as { inputJson: string }).inputJson) as { text: string };
        expect(inner.text).toBe('ambiguous sample');
        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toBe('Banana.');
        expect(screen.queryByTestId('ensemble-retry')).toBeNull();
    });

    test('retry path remains guarded and keeps location stable', async () => {
        let attempts = 0;
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                attempts += 1;
                if (attempts === 1) {
                    return jsonResponse({ error: { message: 'banana service down' } }, 503);
                }
                return jsonResponse({
                    verdict: { label: 'not_banana', score: 0.93, did_escalate: false, calibration_magnitude: 0.22, status: 'ok' },
                    embedding: null,
                });
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();

        const before = getLocationFingerprint();
        await submitEnsemble('retry guarded sample');
        expect(screen.getByTestId('ensemble-retry')).not.toBeNull();

        await act(async () => {
            fireEvent.click(screen.getByTestId('ensemble-retry'));
        });
        await flush();

        const after = getLocationFingerprint();
        expect(didLocationChange(before, after)).toBe(false);
        expect(attempts).toBe(2);
        expect(screen.getByTestId('ensemble-verdict-copy').textContent).toBe('Not a banana.');
    });

    test('editing the input clears the retry button', async () => {
        globalThis.fetch = makeFetchMock((input) => {
            const url = String(input);
            if (url.endsWith('/ml/ensemble/embedding')) {
                return jsonResponse({ error: { message: 'down' } }, 503);
            }
            return defaultBootstrap(input);
        });
        await act(async () => { render(<App />); });
        await flush();

        await submitEnsemble('first sample');
        expect(screen.getByTestId('ensemble-retry')).not.toBeNull();

        const textarea = screen.getByPlaceholderText('Describe a possible banana') as HTMLTextAreaElement;
        await act(async () => {
            fireEvent.change(textarea, { target: { value: 'first sample edited' } });
        });
        await flush();

        expect(screen.queryByTestId('ensemble-retry')).toBeNull();
        expect(screen.queryByTestId('ensemble-error')).toBeNull();
    });
});
