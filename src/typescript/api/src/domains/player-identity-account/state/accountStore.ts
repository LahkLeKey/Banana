export type PlayerAccountRecord = {
  playerId: string;
  accountStatus: 'active' | 'suspended' | 'restricted' | 'deleted';
  profile: Record<string, unknown>;
  version: number;
  updatedAt: string;
};

const accountRecords = new Map<string, PlayerAccountRecord>();

function nowIso(): string {
  return new Date().toISOString();
}

export function getOrCreatePlayerAccount(playerId: string):
    PlayerAccountRecord {
  const existing = accountRecords.get(playerId);
  if (existing) {
    return existing;
  }

  const created: PlayerAccountRecord = {
    playerId,
    accountStatus: 'active',
    profile: {},
    version: 0,
    updatedAt: nowIso(),
  };
  accountRecords.set(playerId, created);
  return created;
}

export function updatePlayerAccount(
    playerId: string,
    profilePatch: Record<string, unknown>,
    ): PlayerAccountRecord {
  const current = getOrCreatePlayerAccount(playerId);
  const updated: PlayerAccountRecord = {
    ...current,
    profile: {
      ...current.profile,
      ...profilePatch,
    },
    version: current.version + 1,
    updatedAt: nowIso(),
  };
  accountRecords.set(playerId, updated);
  return updated;
}

export function resetPlayerAccountStore(): void {
  accountRecords.clear();
}
