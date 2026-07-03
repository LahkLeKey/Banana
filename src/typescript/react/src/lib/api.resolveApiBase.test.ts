import {describe, expect, it} from 'bun:test';

import {resolveApiBaseResolution} from './api';

describe('resolveApiBaseResolution', () => {
  it('keeps explicit non-vercel vite API base URL', () => {
    const resolved =
        resolveApiBaseResolution('https://api.banana.engineer', '');
    expect(resolved.baseUrl).toBe('https://api.banana.engineer');
    expect(resolved.source).toBe('vite');
    expect(resolved.error).toBeNull();
  });

  it('rewrites vercel preview host to production API host', () => {
    const resolved = resolveApiBaseResolution(
        'https://banana-git-branch-user.vercel.app', '');
    expect(resolved.baseUrl).toBe('https://api.banana.engineer');
    expect(resolved.source).toBe('vite');
    expect(resolved.error).toBeNull();
  });

  it('falls back to localhost default when vite base URL missing', () => {
    const resolved = resolveApiBaseResolution('', 'http://localhost:8081');
    expect(resolved.baseUrl).toBe('http://localhost:8081');
    expect(resolved.source).toBe('localhost-default');
    expect(resolved.error).toBeNull();
  });
});
