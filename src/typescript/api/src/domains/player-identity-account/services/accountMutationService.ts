import {recordDomainChange} from '../../gameplay-session-orchestration/persistence/domainChangeRecorder.ts';
import {updatePlayerAccount} from '../state/accountStore.ts';

export type AccountMutationInput = {
  playerId: string; profilePatch: Record<string, unknown>;
  correlationId: string;
  actorScope: {actorId: string; actorType: string};
};

export function mutatePlayerAccount(input: AccountMutationInput): {
  accountStatus: 'active'|'suspended'|'restricted'|'deleted'; version: number;
} {
  const updated = updatePlayerAccount(input.playerId, input.profilePatch);

  recordDomainChange({
    boundedContext: 'player-identity-account',
    actionType: 'account.profile-updated',
    actorScope: input.actorScope,
    beforeStateDigest: input.playerId,
    afterStateDigest: `${updated.playerId}:${updated.version}`,
    correlationId: input.correlationId,
  });

  return {
    accountStatus: updated.accountStatus,
    version: updated.version,
  };
}
