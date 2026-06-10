import type {FastifyInstance} from 'fastify';

import {enforceAccountExpectedVersion} from '../../domains/player-identity-account/pipelines/accountConflictPipeline.ts';
import {mutatePlayerAccount} from '../../domains/player-identity-account/services/accountMutationService.ts';
import {queryPlayerAccount} from '../../domains/player-identity-account/services/accountQueryService.ts';
import {buildPlayerInsightProjection} from '../../domains/player-insights-web/services/insightProjectionService.ts';
import {queryPlayerInventory} from '../../domains/progression-inventory/services/inventoryQueryService.ts';
import {queryPlayerProgression} from '../../domains/progression-inventory/services/progressionQueryService.ts';
import {validateUnifiedPlayerTruthModel} from '../../domains/progression-inventory/validation/playerTruthModelValidator.ts';
import {PlayerAccountUpdateRequestSchema} from '../../lib/contracts/v1/player.ts';
import {assertPlayerSelfScope} from '../../middleware/playerScopeAuthorization.ts';

import {registerV1AccountLinkRemediationRoutes} from './account-link-remediation.ts';
import {registerV1LaunchGateRoutes} from './launch-gate.ts';
import {registerV1PlayerTrainingEconomyRoutes} from './player.training-economy.ts';

export async function registerV1PlayerRoutes(app: FastifyInstance):
    Promise<void> {
  await app.register(async (scope) => {
    scope.get('/account', async (request) => {
      const playerId = request.requestContext.actorScope.actorId;
      assertPlayerSelfScope(request, playerId);
      return queryPlayerAccount(playerId);
    });

    scope.patch('/account', async (request, reply) => {
      const playerId = request.requestContext.actorScope.actorId;
      assertPlayerSelfScope(request, playerId);

      const parsed = PlayerAccountUpdateRequestSchema.safeParse(request.body);
      if (!parsed.success) {
        return reply.status(400).send({
          error: {
            code: 'validation_error',
            message: 'invalid_request',
            correlationId: request.requestContext.correlationId,
            details: parsed.error.flatten(),
          },
        });
      }

      enforceAccountExpectedVersion(playerId, parsed.data.expectedVersion);
      return mutatePlayerAccount({
        playerId,
        profilePatch: parsed.data.profilePatch,
        correlationId: request.requestContext.correlationId,
        actorScope: request.requestContext.actorScope,
      });
    });

    scope.get('/progression', async (request) => {
      const playerId = request.requestContext.actorScope.actorId;
      assertPlayerSelfScope(request, playerId);
      validateUnifiedPlayerTruthModel(playerId);
      return queryPlayerProgression(playerId);
    });

    scope.get('/inventory', async (request) => {
      const playerId = request.requestContext.actorScope.actorId;
      assertPlayerSelfScope(request, playerId);
      validateUnifiedPlayerTruthModel(playerId);
      return queryPlayerInventory(playerId);
    });

    scope.get('/insights', async (request) => {
      const playerId = request.requestContext.actorScope.actorId;
      assertPlayerSelfScope(request, playerId);
      validateUnifiedPlayerTruthModel(playerId);
      return buildPlayerInsightProjection(playerId);
    });

    await scope.register(registerV1LaunchGateRoutes, {prefix: '/launch-gate'});
    await scope.register(registerV1AccountLinkRemediationRoutes, {
      prefix: '/account-link-remediation',
    });
    await scope.register(registerV1PlayerTrainingEconomyRoutes, {
      prefix: '/training',
    });
  }, {prefix: '/v1/player'});
}
