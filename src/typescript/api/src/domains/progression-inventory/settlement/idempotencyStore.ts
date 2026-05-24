export type IdempotencySettlementRecord<T> = {
  key: string; scope: string; payloadHash: string; response: T;
  createdAt: number;
};

export type IdempotencyStore<T> = {
  get: (scope: string, key: string) =>
      Promise<IdempotencySettlementRecord<T>|null>;
  set: (record: IdempotencySettlementRecord<T>) => Promise<void>;
};

export class InMemoryIdempotencyStore<T> implements IdempotencyStore<T> {
  private readonly records = new Map<string, IdempotencySettlementRecord<T>>();

  async get(
      scope: string,
      key: string,
      ): Promise<IdempotencySettlementRecord<T>|null> {
    const found = this.records.get(`${scope}::${key}`);
    return found ?? null;
  }

  async set(record: IdempotencySettlementRecord<T>): Promise<void> {
    this.records.set(`${record.scope}::${record.key}`, record);
  }
}
