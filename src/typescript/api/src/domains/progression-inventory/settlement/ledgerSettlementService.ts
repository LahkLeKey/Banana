import {createHash} from 'node:crypto';

import {type IdempotencySettlementRecord, type IdempotencyStore, InMemoryIdempotencyStore,} from './idempotencyStore.ts';

export type SettlementInput = {
  scope: string; key: string; payload: unknown;
};

export class LedgerSettlementService<T> {
  constructor(private readonly store: IdempotencyStore<T>) {}

  public async settle(
      input: SettlementInput,
      writer: () => Promise<T>,
      ): Promise<{replayed: boolean; value: T}> {
    const payloadHash = createHash('sha256')
                            .update(JSON.stringify(input.payload))
                            .digest('hex');
    const existing = await this.store.get(input.scope, input.key);
    if (existing) {
      return {replayed: true, value: existing.response};
    }

    const response = await writer();
    const record: IdempotencySettlementRecord<T> = {
      key: input.key,
      scope: input.scope,
      payloadHash,
      response,
      createdAt: Date.now(),
    };
    await this.store.set(record);

    return {replayed: false, value: response};
  }
}

export function createDefaultLedgerSettlementService<T>():
    LedgerSettlementService<T> {
  return new LedgerSettlementService(new InMemoryIdempotencyStore<T>());
}
