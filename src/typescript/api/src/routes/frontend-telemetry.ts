import type {FastifyInstance} from 'fastify';
import {z} from 'zod';

const ComponentErrorTelemetrySchema = z.object({
  componentName: z.string().trim().min(1).max(200),
  message: z.string().trim().min(1).max(4000),
  stack: z.string().trim().max(24000).optional(),
  componentStack: z.string().trim().max(24000).optional(),
  routePath: z.string().trim().max(1024).optional(),
  userAgent: z.string().trim().max(2048).optional(),
  timestamp: z.number().int().positive(),
  metadata: z.record(z.unknown()).optional(),
});

export async function registerFrontendTelemetryRoutes(app: FastifyInstance) {
  app.post('/telemetry/component-errors', async (request, reply) => {
    const parsed = ComponentErrorTelemetrySchema.safeParse(request.body);
    if (!parsed.success) {
      return reply.status(400).send({
        error: {
          code: 'validation_error',
          message: 'frontend_component_error_telemetry_invalid_payload',
          correlationId: request.requestContext.correlationId,
          details: parsed.error.flatten(),
        },
      });
    }

    app.log.error(
        {
          ...parsed.data,
          correlationId: request.requestContext.correlationId,
        },
        'frontend component error telemetry event');

    return reply.status(202).send({accepted: true});
  });
}