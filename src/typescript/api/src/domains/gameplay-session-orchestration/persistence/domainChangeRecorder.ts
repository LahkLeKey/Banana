export type DomainChangeRecordInput = {
  boundedContext:|'gameplay-session-orchestration'|'player-identity-account'|
  'progression-inventory'|'player-insights-web';
  actionType: string;
  actorScope: {actorId: string; actorType: string};
  beforeStateDigest: string;
  afterStateDigest: string;
  correlationId: string;
};

export type DomainChangeRecord = DomainChangeRecordInput&{
  changeId: string;
  recordedAt: string;
};

const records: DomainChangeRecord[] = [];

export function recordDomainChange(input: DomainChangeRecordInput):
    DomainChangeRecord {
  const created: DomainChangeRecord = {
    ...input,
    changeId: crypto.randomUUID(),
    recordedAt: new Date().toISOString(),
  };
  records.push(created);
  return created;
}

export function getDomainChangeRecords(): DomainChangeRecord[] {
  return [...records];
}

export function resetDomainChangeRecords(): void {
  records.length = 0;
}
