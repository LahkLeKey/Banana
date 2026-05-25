import {describe, expect, it} from 'bun:test';

import {GameplayAdmissionRequestSchema, GameplayCommandRequestSchema} from './gameplay.ts';
import {RevisitBaselineRequestSchema} from './persistentWorld.ts';
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

  it('validates persistent world replay baseline request shape', () => {
    const parsed = RevisitBaselineRequestSchema.safeParse({
      worldId: '550e8400-e29b-41d4-a716-446655440000',
      laneId: 'primary',
      chunkX: 1,
      chunkY: -2,
      partitionEpoch: 0,
    });
    expect(parsed.success).toBe(true);
  });
});
