import type {AreaIdentity, AreaStateVersion} from '../../../lib/contracts/v1/persistentWorld.ts';

import {assertAllowedAuthoritativeBaselinePayload} from './authoritativePayloadPolicy.ts';

export type AuthoritativeBaselineRecord = {
  baselineId: string; areaIdentity: AreaIdentity;
  areaStateVersion: AreaStateVersion;
  baselinePayload: Record<string, unknown>;
  updatedAt: string;
};

export interface BaselineRepository {
  upsert(record: AuthoritativeBaselineRecord): AuthoritativeBaselineRecord;
  getByAreaId(areaId: string): AuthoritativeBaselineRecord|null;
  getByBaselineId(baselineId: string): AuthoritativeBaselineRecord|null;
  listByLane(worldId: string, laneId: string): AuthoritativeBaselineRecord[];
  reset(): void;
}

class InMemoryBaselineRepository implements BaselineRepository {
  private readonly byAreaId = new Map<string, AuthoritativeBaselineRecord>();
  private readonly byBaselineId =
      new Map<string, AuthoritativeBaselineRecord>();

  upsert(record: AuthoritativeBaselineRecord): AuthoritativeBaselineRecord {
    assertAllowedAuthoritativeBaselinePayload(record.baselinePayload);

    const stored: AuthoritativeBaselineRecord = {
      ...record,
      updatedAt: new Date().toISOString(),
    };

    this.byAreaId.set(stored.areaIdentity.areaId, stored);
    this.byBaselineId.set(stored.baselineId, stored);
    return stored;
  }

  getByAreaId(areaId: string): AuthoritativeBaselineRecord|null {
    return this.byAreaId.get(areaId) ?? null;
  }

  getByBaselineId(baselineId: string): AuthoritativeBaselineRecord|null {
    return this.byBaselineId.get(baselineId) ?? null;
  }

  listByLane(worldId: string, laneId: string): AuthoritativeBaselineRecord[] {
    const rows: AuthoritativeBaselineRecord[] = [];

    for (const value of this.byAreaId.values()) {
      if (value.areaIdentity.worldId === worldId &&
          value.areaIdentity.laneId === laneId) {
        rows.push(value);
      }
    }

    rows.sort(
        (left, right) =>
            left.areaIdentity.areaId.localeCompare(right.areaIdentity.areaId));
    return rows;
  }

  reset(): void {
    this.byAreaId.clear();
    this.byBaselineId.clear();
  }
}

const baselineRepository = new InMemoryBaselineRepository();

export function createBaselineRepository(): BaselineRepository {
  return baselineRepository;
}

export function resetBaselineRepositoryForTests(): void {
  baselineRepository.reset();
}
