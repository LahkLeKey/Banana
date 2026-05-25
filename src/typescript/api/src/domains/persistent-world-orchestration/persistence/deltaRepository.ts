import {assertAllowedAuthoritativeDeltaPayload} from './authoritativePayloadPolicy.ts';

export type OrderedInteractionDeltaRecord = {
  deltaId: string; areaId: string; baseAreaStateVersion: number;
  sequence: number;
  idempotencyKey: string;
  deltaPayload: Record<string, unknown>;
  createdAt: string;
};

export type AppendDeltaInput = {
  areaId: string; baseAreaStateVersion: number; idempotencyKey: string;
  deltaPayload: Record<string, unknown>;
};

export interface DeltaRepository {
  append(input: AppendDeltaInput): OrderedInteractionDeltaRecord;
  listByAreaId(areaId: string): OrderedInteractionDeltaRecord[];
  getByIdempotencyKey(areaId: string, idempotencyKey: string):
      OrderedInteractionDeltaRecord|null;
  highestSequence(areaId: string): number;
  reset(): void;
}

class InMemoryDeltaRepository implements DeltaRepository {
  private readonly byArea = new Map<string, OrderedInteractionDeltaRecord[]>();
  private readonly byIdempotencyKey =
      new Map<string, OrderedInteractionDeltaRecord>();

  append(input: AppendDeltaInput): OrderedInteractionDeltaRecord {
    const dedupeKey = `${input.areaId}:${input.idempotencyKey}`;
    const existing = this.byIdempotencyKey.get(dedupeKey);
    if (existing) {
      return existing;
    }

    assertAllowedAuthoritativeDeltaPayload(input.deltaPayload);

    const nextSequence = this.highestSequence(input.areaId) + 1;
    const record: OrderedInteractionDeltaRecord = {
      deltaId: `${input.areaId}:${nextSequence}`,
      areaId: input.areaId,
      baseAreaStateVersion: input.baseAreaStateVersion,
      sequence: nextSequence,
      idempotencyKey: input.idempotencyKey,
      deltaPayload: input.deltaPayload,
      createdAt: new Date().toISOString(),
    };

    const prior = this.byArea.get(input.areaId) ?? [];
    this.byArea.set(input.areaId, [...prior, record]);
    this.byIdempotencyKey.set(dedupeKey, record);
    return record;
  }

  listByAreaId(areaId: string): OrderedInteractionDeltaRecord[] {
    const rows = [...(this.byArea.get(areaId) ?? [])];
    rows.sort((left, right) => left.sequence - right.sequence);
    return rows;
  }

  getByIdempotencyKey(areaId: string, idempotencyKey: string):
      OrderedInteractionDeltaRecord|null {
    return this.byIdempotencyKey.get(`${areaId}:${idempotencyKey}`) ?? null;
  }

  highestSequence(areaId: string): number {
    const rows = this.byArea.get(areaId);
    if (!rows || rows.length === 0) {
      return 0;
    }

    return rows[rows.length - 1]?.sequence ?? 0;
  }

  reset(): void {
    this.byArea.clear();
    this.byIdempotencyKey.clear();
  }
}

const deltaRepository = new InMemoryDeltaRepository();

export function createDeltaRepository(): DeltaRepository {
  return deltaRepository;
}

export function resetDeltaRepositoryForTests(): void {
  deltaRepository.reset();
}
