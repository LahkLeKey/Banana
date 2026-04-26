import {afterEach, describe, expect, test} from 'bun:test';

import {createChatSession, fetchBananaSummary, fetchEnsembleVerdict, resolveApiBaseUrl, resolvePlatformLabel, sendChatMessage,} from './api';

const originalFetch = globalThis.fetch;

afterEach(() => {
  globalThis.fetch = originalFetch;
});

describe('react api client', () => {
  test('resolves API base URL with env first, then bridge', () => {
    expect(resolveApiBaseUrl('http://vite.example', {
      apiBaseUrl: 'http://bridge.example',
      platform: 'linux',
    })).toBe('http://vite.example');

    expect(resolveApiBaseUrl('', {
      apiBaseUrl: 'http://bridge.example',
      platform: 'linux',
    })).toBe('http://bridge.example');

    expect(resolveApiBaseUrl('', undefined)).toBe('');
  });

  test('resolves platform label from bridge', () => {
    expect(resolvePlatformLabel({
      apiBaseUrl: 'http://bridge.example',
      platform: 'win32',
    })).toBe('electron-win32');

    expect(resolvePlatformLabel(undefined)).toBe('web');
  });

  test('fetchBananaSummary returns typed payload', async () => {
    globalThis.fetch = (async () => new Response(JSON.stringify({banana: 42}), {
                          status: 200,
                          headers: {'content-type': 'application/json'},
                        })) as typeof fetch;

    const payload = await fetchBananaSummary('http://api.example');
    expect(payload.banana).toBe(42);
  });

  test('createChatSession posts platform metadata', async () => {
    let lastUrl = '';
    let lastInit: RequestInit|undefined;

    globalThis.fetch = (async (input: RequestInfo|URL, init?: RequestInit) => {
                         lastUrl = String(input);
                         lastInit = init;
                         return new Response(
                             JSON.stringify({
                               session: {
                                 id: 'chat_1',
                                 platform: 'web',
                                 created_at: '2026-04-25T00:00:00Z',
                                 updated_at: '2026-04-25T00:00:00Z',
                                 message_count: 1,
                               },
                               welcome_message: {
                                 id: 'chat_1_m0001',
                                 session_id: 'chat_1',
                                 role: 'assistant',
                                 content: 'banana assistant ready',
                                 created_at: '2026-04-25T00:00:00Z',
                                 status: 'complete',
                               },
                             }),
                             {
                               status: 201,
                               headers: {'content-type': 'application/json'},
                             });
                       }) as typeof fetch;

    const payload = await createChatSession('http://api.example', 'web');

    expect(payload.session.id).toBe('chat_1');
    expect(lastUrl).toBe('http://api.example/chat/sessions');
    expect(lastInit?.method).toBe('POST');

    const body = JSON.parse(String(lastInit?.body)) as {
      platform: string;
      metadata: {source: string};
    };
    expect(body.platform).toBe('web');
    expect(body.metadata.source).toBe('react-portal');
  });

  test('sendChatMessage posts content and idempotency key', async () => {
    let lastUrl = '';
    let lastInit: RequestInit|undefined;

    globalThis.fetch = (async (input: RequestInfo|URL, init?: RequestInit) => {
                         lastUrl = String(input);
                         lastInit = init;
                         return new Response(
                             JSON.stringify({
                               session_id: 'chat_1',
                               duplicate: false,
                               user_message: {
                                 id: 'chat_1_m0002',
                                 session_id: 'chat_1',
                                 role: 'user',
                                 content: 'banana question',
                                 created_at: '2026-04-25T00:00:00Z',
                                 status: 'accepted',
                               },
                               assistant_message: {
                                 id: 'chat_1_m0003',
                                 session_id: 'chat_1',
                                 role: 'assistant',
                                 content: 'banana reply',
                                 created_at: '2026-04-25T00:00:00Z',
                                 status: 'complete',
                               },
                             }),
                             {
                               status: 201,
                               headers: {'content-type': 'application/json'},
                             });
                       }) as typeof fetch;

    const payload = await sendChatMessage(
        'http://api.example', 'chat_1', 'banana question', 'client-1');

    expect(payload.session_id).toBe('chat_1');
    expect(lastUrl).toBe('http://api.example/chat/sessions/chat_1/messages');
    expect(lastInit?.method).toBe('POST');

    const body = JSON.parse(String(lastInit?.body)) as {
      content: string;
      client_message_id: string;
    };
    expect(body.content).toBe('banana question');
    expect(body.client_message_id).toBe('client-1');
  });

  test('propagates API errors with typed message', async () => {
    globalThis.fetch =
        (async () => new Response(
             JSON.stringify({error: {message: 'banana backend down'}}), {
               status: 503,
               headers: {'content-type': 'application/json'},
             })) as typeof fetch;

    await expect(fetchBananaSummary('http://api.example'))
        .rejects.toThrow('banana backend down');
  });

  test(
      'fetchEnsembleVerdict snapshots the slice 014 contract shape',
      async () => {
        let lastUrl = '';
        let lastInit: RequestInit|undefined;

        globalThis.fetch =
            (async (input: RequestInfo|URL, init?: RequestInit) => {
              lastUrl = String(input);
              lastInit = init;
              return new Response(
                  JSON.stringify({
                    label: 'banana',
                    score: 0.83,
                    did_escalate: true,
                    calibration_magnitude: 0.6,
                    status: 'ok',
                  }),
                  {
                    status: 200,
                    headers: {'content-type': 'application/json'},
                  });
            }) as typeof fetch;

        const verdict = await fetchEnsembleVerdict(
            'http://api.example', 'yellow fruit on the counter maybe');

        // Field-by-field snapshot: any drift in slice 014 wire shape breaks
        // this test (R-R03 contract).
        expect(Object.keys(verdict).sort()).toEqual([
          'calibration_magnitude',
          'did_escalate',
          'label',
          'score',
          'status',
        ]);
        expect(verdict.label).toBe('banana');
        expect(verdict.score).toBe(0.83);
        expect(verdict.did_escalate).toBe(true);
        expect(verdict.calibration_magnitude).toBe(0.6);
        expect(verdict.status).toBe('ok');

        expect(lastUrl).toBe('http://api.example/ml/ensemble');
        expect(lastInit?.method).toBe('POST');
        const body = JSON.parse(String(lastInit?.body)) as {inputJson: string};
        const inner = JSON.parse(body.inputJson) as {text: string};
        expect(inner.text).toBe('yellow fruit on the counter maybe');
      });
});
