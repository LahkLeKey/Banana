import type {FastifyInstance} from 'fastify';

import {settleGameplayMutation} from '../../domains/gameplay-session-orchestration/pipelines/settlementPipeline.ts';
import {admitPlayerToSession} from '../../domains/gameplay-session-orchestration/services/admissionService.ts';
import {runAuthoritativeCommand} from '../../domains/gameplay-session-orchestration/services/commandService.ts';
import {restoreSessionContinuity} from '../../domains/gameplay-session-orchestration/services/rejoinService.ts';
import {terminateSession} from '../../domains/gameplay-session-orchestration/services/terminationService.ts';
import {GameplayAdmissionRequestSchema, GameplayCommandRequestSchema, GameplayRejoinRequestSchema, GameplayTerminateRequestSchema,} from '../../lib/contracts/v1/gameplay.ts';
import {authenticationRequired} from '../../lib/errors/domainErrors.ts';

export async function registerV1GameplayRoutes(app: FastifyInstance):
    Promise<void> {
  await app.register(async (scope) => {
    scope.post('/sessions/admissions', async (request, reply) => {
      const parsed = GameplayAdmissionRequestSchema.safeParse(request.body);
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

      if (request.requestContext.actorScope.actorType === 'unknown') {
        throw authenticationRequired('actor_scope_required');
      }

      const settled = await settleGameplayMutation(
          `admission:${parsed.data.playerId}`, parsed.data.idempotencyKey,
          parsed.data, () => admitPlayerToSession({
                         playerId: parsed.data.playerId,
                         queueIntent: parsed.data.queueIntent,
                         actorScope: request.requestContext.actorScope,
                         correlationId: request.requestContext.correlationId,
                       }));

      return reply.status(settled.replayed ? 200 : 201).send({
        sessionId: settled.value.sessionId,
        admissionStatus: settled.value.admissionStatus,
        continuityWindowExpiresAt: settled.value.continuityWindowExpiresAt,
      });
    });

    scope.post('/sessions/:sessionId/commands', async (request, _reply) => {
      const parsed = GameplayCommandRequestSchema.safeParse(request.body);
      if (!parsed.success) {
        return {
          error: {
            code: 'validation_error',
            message: 'invalid_request',
            correlationId: request.requestContext.correlationId,
            details: parsed.error.flatten(),
          },
        };
      }
      const sessionId = (request.params as {sessionId: string}).sessionId;

      const settled = await settleGameplayMutation(
          `command:${sessionId}:${parsed.data.playerId}`,
          parsed.data.idempotencyKey, parsed.data,
          () => runAuthoritativeCommand({
            sessionId,
            playerId: parsed.data.playerId,
            commandType: parsed.data.commandType,
            commandPayload: parsed.data.commandPayload,
            commandSequence: parsed.data.commandSequence,
            actorScope: request.requestContext.actorScope,
            correlationId: request.requestContext.correlationId,
          }));

      return settled.value;
    });

    scope.post('/sessions/:sessionId/rejoin', async (request, _reply) => {
      const parsed = GameplayRejoinRequestSchema.safeParse(request.body);
      if (!parsed.success) {
        return {
          error: {
            code: 'validation_error',
            message: 'invalid_request',
            correlationId: request.requestContext.correlationId,
            details: parsed.error.flatten(),
          },
        };
      }
      const sessionId = (request.params as {sessionId: string}).sessionId;
      return restoreSessionContinuity(
          sessionId, parsed.data.playerId, parsed.data.continuityToken);
    });

    scope.post('/sessions/:sessionId/terminate', async (request, _reply) => {
      const parsed = GameplayTerminateRequestSchema.safeParse(request.body);
      if (!parsed.success) {
        return {
          error: {
            code: 'validation_error',
            message: 'invalid_request',
            correlationId: request.requestContext.correlationId,
            details: parsed.error.flatten(),
          },
        };
      }
      const sessionId = (request.params as {sessionId: string}).sessionId;
      const settled = await settleGameplayMutation(
          `terminate:${sessionId}:${parsed.data.playerId}`,
          parsed.data.idempotencyKey, parsed.data,
          () => terminateSession(sessionId, parsed.data.terminationReason));
      return settled.value;
    });
  }, {prefix: '/v1/gameplay'});
}
