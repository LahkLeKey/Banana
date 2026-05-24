import {createDefaultLedgerSettlementService, type LedgerSettlementService,} from '../../progression-inventory/settlement/ledgerSettlementService.ts';

type SettledResponse<T> = {
  replayed: boolean; value: T;
};

let settlementService: LedgerSettlementService<unknown>|null = null;

function getSettlementService(): LedgerSettlementService<unknown> {
  if (!settlementService) {
    settlementService = createDefaultLedgerSettlementService<unknown>();
  }
  return settlementService;
}

export async function settleGameplayMutation<T>(
    scope: string,
    idempotencyKey: string,
    payload: unknown,
    writer: () => Promise<T>,
    ): Promise<SettledResponse<T>> {
  const settled = await getSettlementService().settle(
      {
        scope,
        key: idempotencyKey,
        payload,
      },
      async () => writer() as Promise<unknown>);
  return {
    replayed: settled.replayed,
    value: settled.value as T,
  };
}

export function __resetSettlementPipelineForTests(): void {
  settlementService = null;
}
