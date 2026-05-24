export type ProgressionSnapshot = {
  playerId: string; progression: Record<string, unknown>;
  freshnessTimestamp: string;
  snapshotRef: string;
};

export type InventoryItem = Record<string, unknown>;

export type InventorySnapshot = {
  playerId: string; inventory: InventoryItem[]; freshnessTimestamp: string;
  snapshotRef: string;
};

type ProgressionState = {
  xp: number; level: number; updatedAt: string;
};

type InventoryStateItem = {
  itemId: string; quantity: number;
};

const progressionByPlayer = new Map<string, ProgressionState>();
const inventoryByPlayer = new Map<string, InventoryStateItem[]>();

function nowIso(): string {
  return new Date().toISOString();
}

function ensureProgression(playerId: string): ProgressionState {
  const existing = progressionByPlayer.get(playerId);
  if (existing) {
    return existing;
  }

  const created = {xp: 0, level: 1, updatedAt: nowIso()};
  progressionByPlayer.set(playerId, created);
  return created;
}

function ensureInventory(playerId: string): InventoryStateItem[] {
  const existing = inventoryByPlayer.get(playerId);
  if (existing) {
    return existing;
  }

  const created: InventoryStateItem[] = [];
  inventoryByPlayer.set(playerId, created);
  return created;
}

function levelFromXp(xp: number): number {
  return Math.max(1, Math.floor(xp / 100) + 1);
}

export function applyGameplayCommandMutation(
    playerId: string,
    payload: Record<string, unknown>,
    ): void {
  const progression = ensureProgression(playerId);
  const inventory = ensureInventory(playerId);

  const xpDelta = Number(payload.xpDelta ?? 0);
  if (Number.isFinite(xpDelta) && xpDelta !== 0) {
    progression.xp = Math.max(0, progression.xp + Math.trunc(xpDelta));
    progression.level = levelFromXp(progression.xp);
    progression.updatedAt = nowIso();
  }

  const itemId = typeof payload.itemId === 'string' ? payload.itemId : '';
  const quantityDelta = Number(payload.quantityDelta ?? 0);
  if (itemId && Number.isFinite(quantityDelta) && quantityDelta !== 0) {
    const item = inventory.find((entry) => entry.itemId === itemId);
    if (item) {
      item.quantity = Math.max(0, item.quantity + Math.trunc(quantityDelta));
    } else if (quantityDelta > 0) {
      inventory.push({itemId, quantity: Math.trunc(quantityDelta)});
    }
  }

  const timestamp = nowIso();
  progression.updatedAt = timestamp;
}

export function queryProgressionSnapshot(playerId: string):
    ProgressionSnapshot {
  const progression = ensureProgression(playerId);
  return {
    playerId,
    progression: {
      xp: progression.xp,
      level: progression.level,
    },
    freshnessTimestamp: progression.updatedAt,
    snapshotRef: `${playerId}:progression:${progression.updatedAt}`,
  };
}

export function queryInventorySnapshot(playerId: string): InventorySnapshot {
  const inventory = ensureInventory(playerId)
                        .filter((entry) => entry.quantity > 0)
                        .map((entry) => ({
                               itemId: entry.itemId,
                               quantity: entry.quantity,
                             }));
  const freshnessTimestamp = nowIso();
  return {
    playerId,
    inventory,
    freshnessTimestamp,
    snapshotRef: `${playerId}:inventory:${freshnessTimestamp}`,
  };
}

export function resetPlayerTruthStore(): void {
  progressionByPlayer.clear();
  inventoryByPlayer.clear();
}
