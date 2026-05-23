// @ts-nocheck -- bun:test types are not part of app tsconfig.

import {GlobalRegistrator} from '@happy-dom/global-registrator';
import {describe, expect, test} from 'bun:test';

import {BANANA_AUTH_STEAM_ID_STORAGE_KEY, BANANA_AUTH_TOKEN_STORAGE_KEY, buildSteamAuthStartUrl, clearStoredAuthSession, parseAuthCallbackHash, readStoredAuthSession, storeAuthSession,} from './session';

describe('auth session helpers', () => {
  test('parses callback hashes with token and steam id', () => {
    const session =
        parseAuthCallbackHash('#auth_token=abc123&steam_id=76561198000000000');
    expect(session).toEqual({token: 'abc123', steamId: '76561198000000000'});
  });

  test('builds the Steam start url with a return target', () => {
    const url = buildSteamAuthStartUrl(
        'https://api.banana.engineer', 'https://banana.engineer/login');
    expect(url).toBe(
        'https://api.banana.engineer/auth/steam/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin');
  });

  test('stores and clears auth sessions in localStorage', () => {
    if (!(globalThis as unknown as {document?: Document}).document) {
      GlobalRegistrator.register({url: 'http://localhost:5173/'});
    }

    storeAuthSession({token: 'tok-1', steamId: 'steam-1'});
    expect(readStoredAuthSession())
        .toEqual({token: 'tok-1', steamId: 'steam-1'});
    expect(window.localStorage.getItem(BANANA_AUTH_TOKEN_STORAGE_KEY))
        .toBe('tok-1');
    expect(window.localStorage.getItem(BANANA_AUTH_STEAM_ID_STORAGE_KEY))
        .toBe('steam-1');

    clearStoredAuthSession();
    expect(readStoredAuthSession()).toBeNull();
  });
});
