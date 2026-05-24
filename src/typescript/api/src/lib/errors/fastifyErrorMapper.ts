import type {FastifyInstance} from 'fastify';

import {DomainError} from './domainErrors.ts';

export function registerFastifyErrorMapper(app: FastifyInstance): void {
  app.setErrorHandler((error, request, reply) => {
    const correlationId = request.requestContext?.correlationId ?? request.id;
    if (error instanceof DomainError) {
      reply.status(error.statusCode).send({
        error: {
          code: error.code,
          message: error.message,
          correlationId,
          details: error.details,
        },
      });
      return;
    }

    reply.status(500).send({
      error: {
        code: 'dependency_unavailable',
        message: error instanceof Error ? error.message :
                                          'Unhandled server error',
        correlationId,
      },
    });
  });
}
