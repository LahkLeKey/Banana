import {describe, expect, it} from 'bun:test';

import {GameplayAdmissionRequestSchema, GameplayCommandRequestSchema} from './gameplay.ts';
import {PlayerAccountViewSchema} from './player.ts';

describe('v1 contract schemas', () => {
  it('validates gameplay admission requests', () => {
    const parsed = GameplayAdmissionRequestSchema.safeParse({
      playerId: 'player-1',
      queueIntent: 'ranked',
      idempotencyKey: 'idem-12345678',
    });
    expect(parsed.success).toBe(true);
  });

  it('rejects gameplay command requests with negative command sequence', () => {
    const parsed = GameplayCommandRequestSchema.safeParse({
      playerId: 'player-1',
      commandType: 'move',
      commandPayload: {x: 1},
      commandSequence: -1,
      idempotencyKey: 'idem-12345678',
    });
    expect(parsed.success).toBe(false);
  });

  it('validates player account response shape', () => {
    const parsed = PlayerAccountViewSchema.safeParse({
      playerId: 'player-1',
      accountStatus: 'active',
      profile: {},
      version: 1,
    });
    expect(parsed.success).toBe(true);
  });
});
