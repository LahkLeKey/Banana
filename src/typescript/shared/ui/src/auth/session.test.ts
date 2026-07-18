// @ts-nocheck -- bun:test types are not part of app tsconfig.

import {GlobalRegistrator} from '@happy-dom/global-registrator';
import {describe, expect, test} from 'bun:test';

import {BANANA_AUTH_SUBJECT_STORAGE_KEY, BANANA_AUTH_TOKEN_STORAGE_KEY, buildAuthStartUrl, clearStoredAuthSession, parseAuthCallbackHash, readStoredAuthSession, storeAuthSession,} from './session';

describe('auth session helpers', () => {
  test('parses callback hashes with token and subject', () => {
    const session =
        parseAuthCallbackHash('#auth_token=abc123&subject=player:abc');
    expect(session).toEqual({token: 'abc123', subject: 'player:abc'});
  });

  test('builds the Keycloak start url with a return target', () => {
    const url = buildAuthStartUrl(
        'https://api.banana.engineer', 'https://banana.engineer/login');
    expect(url).toBe(
        'https://api.banana.engineer/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin');
  });

  test('builds provider-aware Keycloak start url', () => {
    const url = buildAuthStartUrl(
        'https://api.banana.engineer', 'https://banana.engineer/login',
        'github');
    expect(url).toBe(
        'https://api.banana.engineer/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin&provider=github');
  });

  test('builds Keycloak start url with register action', () => {
    const url = buildAuthStartUrl(
        'https://api.banana.engineer', 'https://banana.engineer/login',
        undefined, 'register');
    expect(url).toBe(
        'https://api.banana.engineer/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin&action=register');
  });

  test('builds Keycloak start url with reset-password action', () => {
    const url = buildAuthStartUrl(
        'https://api.banana.engineer', 'https://banana.engineer/login',
        undefined, 'reset-password');
    expect(url).toBe(
        'https://api.banana.engineer/auth/keycloak/start?returnTo=https%3A%2F%2Fbanana.engineer%2Flogin&action=reset-password');
  });

  test('stores and clears auth sessions in memory', () => {
    if (!(globalThis as unknown as {document?: Document}).document) {
      GlobalRegistrator.register({url: 'http://localhost:5173/'});
    }

    storeAuthSession({token: 'tok-1', subject: 'player-1'});
    expect(readStoredAuthSession())
        .toEqual({token: 'tok-1', subject: 'player-1'});

    // Storage keys remain exported for compatibility across consumers.
    expect(BANANA_AUTH_TOKEN_STORAGE_KEY).toBe('banana-auth-token');
    expect(BANANA_AUTH_SUBJECT_STORAGE_KEY).toBe('banana-auth-subject');

    clearStoredAuthSession();
    expect(readStoredAuthSession()).toBeNull();
  });
});
