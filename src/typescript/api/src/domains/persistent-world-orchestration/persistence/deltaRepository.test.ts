import {beforeEach, describe, expect, test} from 'bun:test';

import {createDeltaRepository, resetDeltaRepositoryForTests} from './deltaRepository.ts';

describe('delta repository', () => {
  beforeEach(() => {
    resetDeltaRepositoryForTests();
  });

  test('append orders sequence and enforces idempotency key', () => {
    const repository = createDeltaRepository();

    const first = repository.append({
      areaId: 'area-1',
      baseAreaStateVersion: 0,
      idempotencyKey: 'k1',
      deltaPayload: {op: 'set'},
    });
    const duplicate = repository.append({
      areaId: 'area-1',
      baseAreaStateVersion: 0,
      idempotencyKey: 'k1',
      deltaPayload: {op: 'set-again'},
    });
    const second = repository.append({
      areaId: 'area-1',
      baseAreaStateVersion: 1,
      idempotencyKey: 'k2',
      deltaPayload: {op: 'clear'},
    });

    expect(first.sequence).toBe(1);
    expect(duplicate.deltaId).toBe(first.deltaId);
    expect(second.sequence).toBe(2);

    const rows = repository.listByAreaId('area-1');
    expect(rows.map((row) => row.sequence)).toEqual([1, 2]);
    expect(repository.highestSequence('area-1')).toBe(2);
  });
});
