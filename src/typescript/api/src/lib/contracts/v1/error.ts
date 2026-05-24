import {z} from 'zod';

export const V1ErrorCodeSchema = z.enum([
  'validation_error',
  'authentication_required',
  'forbidden',
  'conflict',
  'dependency_unavailable',
  'not_found',
]);

export const V1ErrorSchema = z.object({
  error: z.object({
    code: V1ErrorCodeSchema,
    message: z.string(),
    correlationId: z.string().min(1),
    details: z.record(z.unknown()).optional(),
  }),
});

export type V1ErrorCode = z.infer<typeof V1ErrorCodeSchema>;
export type V1Error = z.infer<typeof V1ErrorSchema>;
