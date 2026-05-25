import {type AreaStateVersion, AreaStateVersionSchema} from '../../../lib/contracts/v1/persistentWorld.ts';
import {conflict} from '../../../lib/errors/domainErrors.ts';

export interface AreaStateVersionService {
  ensure(areaId: string, initialSignature: string): AreaStateVersion;
  assertNotStale(areaId: string, candidateBaseVersion: number): void;
  advance(
      areaId: string, appliedDeltaThroughSequence: number,
      signature: string): AreaStateVersion;
  get(areaId: string): AreaStateVersion|null;
  reset(): void;
}

class InMemoryAreaStateVersionService implements AreaStateVersionService {
  private readonly versions = new Map<string, AreaStateVersion>();

  ensure(areaId: string, initialSignature: string): AreaStateVersion {
    const existing = this.versions.get(areaId);
    if (existing) {
      return existing;
    }

    const created = AreaStateVersionSchema.parse({
      areaId,
      areaStateVersion: 0,
      appliedDeltaThroughSequence: 0,
      canonicalStateSignature: initialSignature,
    });
    this.versions.set(areaId, created);
    return created;
  }

  assertNotStale(areaId: string, candidateBaseVersion: number): void {
    const existing = this.versions.get(areaId);
    if (!existing) {
      return;
    }

    if (candidateBaseVersion < existing.areaStateVersion) {
      throw conflict('persistent_world_stale_delta_version', {
        areaId,
        candidateBaseVersion,
        authoritativeAreaStateVersion: existing.areaStateVersion,
      });
    }
  }

  advance(
      areaId: string, appliedDeltaThroughSequence: number,
      signature: string): AreaStateVersion {
    const current = this.ensure(areaId, signature);
    const next = AreaStateVersionSchema.parse({
      areaId,
      areaStateVersion: current.areaStateVersion + 1,
      appliedDeltaThroughSequence,
      canonicalStateSignature: signature,
    });
    this.versions.set(areaId, next);
    return next;
  }

  get(areaId: string): AreaStateVersion|null {
    return this.versions.get(areaId) ?? null;
  }

  reset(): void {
    this.versions.clear();
  }
}

const areaStateVersionService = new InMemoryAreaStateVersionService();

export function createAreaStateVersionService(): AreaStateVersionService {
  return areaStateVersionService;
}

export function resetAreaStateVersionServiceForTests(): void {
  areaStateVersionService.reset();
}
