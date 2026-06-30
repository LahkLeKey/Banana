import {describe, expect, test} from 'bun:test';

import {buildCorsOriginMatcher} from './corsOrigins.ts';

describe('buildCorsOriginMatcher', () => {
  const isAllowed = buildCorsOriginMatcher(
      'https://banana.engineer,https://*.vercel.app,http://localhost:5173');

  test('allows exact origin matches', () => {
    expect(isAllowed('https://banana.engineer')).toBe(true);
    expect(isAllowed('http://localhost:5173')).toBe(true);
  });

  test('allows wildcard preview domains', () => {
    expect(isAllowed('https://banana-ui-preview.vercel.app')).toBe(true);
    expect(isAllowed('https://banana-git-branch-user.vercel.app')).toBe(true);
  });

  test('rejects bare wildcard base domain', () => {
    expect(isAllowed('https://vercel.app')).toBe(false);
  });

  test('rejects protocol mismatches for wildcard domains', () => {
    expect(isAllowed('http://banana-ui-preview.vercel.app')).toBe(false);
  });

  test('rejects non-matching domains', () => {
    expect(isAllowed('https://banana-ui-preview.example.com')).toBe(false);
  });

  test('rejects non-origin values with paths or query', () => {
    expect(isAllowed('https://banana.engineer/some/path')).toBe(false);
    expect(isAllowed('https://banana.engineer?foo=bar')).toBe(false);
  });
});
