import type {V1ErrorCode} from '../contracts/v1/error.ts';

export class DomainError extends Error {
  public readonly code: V1ErrorCode;
  public readonly statusCode: number;
  public readonly details?: Record<string, unknown>;

  constructor(
      code: V1ErrorCode,
      message: string,
      statusCode: number,
      details?: Record<string, unknown>,
  ) {
    super(message);
    this.code = code;
    this.statusCode = statusCode;
    this.details = details;
  }
}

export function validationError(
    message: string, details?: Record<string, unknown>): DomainError {
  return new DomainError('validation_error', message, 400, details);
}

export function authenticationRequired(message = 'Authentication required'):
    DomainError {
  return new DomainError('authentication_required', message, 401);
}

export function forbidden(message = 'Forbidden'): DomainError {
  return new DomainError('forbidden', message, 403);
}

export function conflict(
    message: string, details?: Record<string, unknown>): DomainError {
  return new DomainError('conflict', message, 409, details);
}

export function dependencyUnavailable(message: string): DomainError {
  return new DomainError('dependency_unavailable', message, 503);
}
