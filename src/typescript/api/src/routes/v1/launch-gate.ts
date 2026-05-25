import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

import {createLaunchGateVerificationService} from '../../domains/player-identity-account/services/launchGateVerificationService.ts';

const VerifyLaunchAttemptSchema = z.object({
  attemptId: z.string().uuid().optional(),
  mode: z.string().min(1),
  steamAssertion:
      z.object({
         steamId: z.string().min(1).optional(),
         validationStatus: z.enum([
                              'valid', 'invalid', 'expired', 'indeterminate'
                            ]).optional(),
       }).optional(),
  clientBuildChannel: z.string().min(1).optional(),
  overrideContext: z.record(z.unknown()).optional(),
});

const RetryIntentSchema = z.object({
  attemptId: z.string().uuid(),
  priorReasonCode: z.string().min(1),
  clientObservedAt: z.string().min(1),
});

export async function registerV1LaunchGateRoutes(app: FastifyInstance):
    Promise<void> {
  const verificationService = createLaunchGateVerificationService();

  app.post('/verify', async (request, reply) => {
    const parsed = VerifyLaunchAttemptSchema.safeParse(request.body);
    if (!parsed.success) {
      return reply.status(400).send({
        error: {
          code: 'validation_error',
          message: 'invalid_launch_gate_verify_request',
          correlationId: request.requestContext.correlationId,
          details: parsed.error.flatten(),
        },
      });
    }

    const decision = verificationService.verify(
        {
          attemptId: parsed.data.attemptId,
          mode: parsed.data.mode,
          clientBuildChannel: parsed.data.clientBuildChannel,
          steamAssertion: parsed.data.steamAssertion,
          overrideContext: parsed.data.overrideContext as {
            source?: string;
            allowNonSteamStartup?: boolean;
          } | undefined,
        },
        request.requestContext.correlationId,
    );

    return reply.status(200).send(decision);
  });

  app.get('/status/:attemptId', async (request, reply) => {
    const params =
        z.object({attemptId: z.string().uuid()}).safeParse(request.params);
    if (!params.success) {
      return reply.status(400).send({
        error: {
          code: 'validation_error',
          message: 'invalid_launch_gate_status_request',
          correlationId: request.requestContext.correlationId,
          details: params.error.flatten(),
        },
      });
    }

    const status = verificationService.getStatus(params.data.attemptId);
    if (!status) {
      return reply.status(404).send({
        error: {
          code: 'not_found',
          message: 'launch_attempt_not_found',
          correlationId: request.requestContext.correlationId,
        },
      });
    }

    return reply.status(200).send(status);
  });

  app.post('/retry-intent', async (request, reply) => {
    const parsed = RetryIntentSchema.safeParse(request.body);
    if (!parsed.success) {
      return reply.status(400).send({
        error: {
          code: 'validation_error',
          message: 'invalid_retry_intent_request',
          correlationId: request.requestContext.correlationId,
          details: parsed.error.flatten(),
        },
      });
    }

    const result = verificationService.recordRetryIntent({
      attemptId: parsed.data.attemptId,
      priorReasonCode: parsed.data.priorReasonCode,
      clientObservedAt: parsed.data.clientObservedAt,
    });
    return reply.status(200).send(result);
  });
}
