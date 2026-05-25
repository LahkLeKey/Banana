import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

import {createRemediationTicketService} from '../../domains/player-identity-account/services/remediationTicketService.ts';
import {isLaunchGateReasonCode} from '../../lib/contracts/launchGateReasonCodes.ts';

const CreateRemediationTicketSchema = z.object({
  reasonCode: z.string().min(1),
});

export async function registerV1AccountLinkRemediationRoutes(
    app: FastifyInstance): Promise<void> {
  const remediationService = createRemediationTicketService();

  app.post('/ticket', async (request, reply) => {
    const parsed = CreateRemediationTicketSchema.safeParse(request.body);
    if (!parsed.success) {
      return reply.status(400).send({
        error: {
          code: 'validation_error',
          message: 'invalid_remediation_ticket_request',
          correlationId: request.requestContext.correlationId,
          details: parsed.error.flatten(),
        },
      });
    }

    if (!isLaunchGateReasonCode(parsed.data.reasonCode)) {
      return reply.status(400).send({
        error: {
          code: 'validation_error',
          message: 'invalid_reason_code',
          correlationId: request.requestContext.correlationId,
        },
      });
    }

    const ticket = remediationService.create(parsed.data.reasonCode);
    return reply.status(201).send(ticket);
  });

  app.get('/ticket/:remediationTicketId', async (request, reply) => {
    const params = z.object({
                      remediationTicketId: z.string().uuid()
                    }).safeParse(request.params);
    if (!params.success) {
      return reply.status(400).send({
        error: {
          code: 'validation_error',
          message: 'invalid_remediation_ticket_id',
          correlationId: request.requestContext.correlationId,
          details: params.error.flatten(),
        },
      });
    }

    const ticket = remediationService.get(params.data.remediationTicketId);
    if (!ticket) {
      return reply.status(404).send({
        error: {
          code: 'not_found',
          message: 'remediation_ticket_not_found',
          correlationId: request.requestContext.correlationId,
        },
      });
    }

    return reply.status(200).send(ticket);
  });

  app.post('/ticket/:remediationTicketId/complete', async (request, reply) => {
    const params = z.object({
                      remediationTicketId: z.string().uuid()
                    }).safeParse(request.params);
    if (!params.success) {
      return reply.status(400).send({
        error: {
          code: 'validation_error',
          message: 'invalid_remediation_ticket_id',
          correlationId: request.requestContext.correlationId,
          details: params.error.flatten(),
        },
      });
    }

    const ticket = remediationService.complete(params.data.remediationTicketId);
    if (!ticket) {
      return reply.status(404).send({
        error: {
          code: 'not_found',
          message: 'remediation_ticket_not_found',
          correlationId: request.requestContext.correlationId,
        },
      });
    }

    return reply.status(200).send(ticket);
  });
}
