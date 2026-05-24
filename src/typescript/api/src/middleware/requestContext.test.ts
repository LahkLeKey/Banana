import {describe, expect, test} from 'bun:test';

import {signToken} from './auth.ts';
import {buildRequestContext} from './requestContext.ts';

process.env.BANANA_JWT_SECRET = 'test-secret-banana-jwt-1234';

describe('request context middleware', () => {
  test('maps steam bearer subject to player actor scope', async () => {
    const token = await signToken({
      sub: 'steam:76561198000000000',
      role: 'viewer',
    });

    const request = {
      headers: {
        authorization: `Bearer ${token}`,
      },
    } as unknown as Parameters<typeof buildRequestContext>[0];

    const context = await buildRequestContext(request);
    expect(context.actorScope.actorType).toBe('player');
    expect(context.actorScope.actorId).toBe('76561198000000000');
  });

  test('maps non-steam bearer subject to player actor scope', async () => {
    const token = await signToken({
      sub: 'player-123',
      role: 'viewer',
    });

    const request = {
      headers: {
        authorization: `Bearer ${token}`,
      },
    } as unknown as Parameters<typeof buildRequestContext>[0];

    const context = await buildRequestContext(request);
    expect(context.actorScope.actorType).toBe('player');
    expect(context.actorScope.actorId).toBe('player-123');
  });

  test('treats invalid bearer tokens as anonymous', async () => {
    const request = {
      headers: {
        authorization: 'Bearer invalid.token.value',
      },
    } as unknown as Parameters<typeof buildRequestContext>[0];

    const context = await buildRequestContext(request);
    expect(context.actorScope.actorType).toBe('unknown');
    expect(context.actorScope.actorId).toBe('anonymous');
  });
});