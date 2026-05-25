import {validationError} from '../../../lib/errors/domainErrors.ts';

const FORBIDDEN_MESH_KEYS = new Set([
  'baselineHeightfield',
  'baselineMesh',
  'baselineMeshBlob',
  'heightfield',
  'heights',
  'indices',
  'mesh',
  'meshBlob',
  'normals',
  'uvs',
  'vertices',
]);

function isBinaryLike(value: unknown): boolean {
  return value instanceof ArrayBuffer || ArrayBuffer.isView(value);
}

function assertNoMeshLikePayload(
    value: unknown,
    path: string[] = [],
    ): void {
  if (isBinaryLike(value)) {
    throw validationError('persistent_world_mesh_blob_not_allowed', {
      path: path.join('.'),
      reason: 'binary_payload',
    });
  }

  if (Array.isArray(value)) {
    for (let index = 0; index < value.length; index++) {
      assertNoMeshLikePayload(value[index], [...path, String(index)]);
    }
    return;
  }

  if (!value || typeof value !== 'object') {
    return;
  }

  for (const [key, nested] of Object.entries(
           value as Record<string, unknown>)) {
    if (FORBIDDEN_MESH_KEYS.has(key)) {
      throw validationError('persistent_world_mesh_blob_not_allowed', {
        path: [...path, key].join('.'),
        reason: 'mesh_like_field',
      });
    }

    assertNoMeshLikePayload(nested, [...path, key]);
  }
}

export function assertAllowedAuthoritativeBaselinePayload(
    payload: Record<string, unknown>,
    ): void {
  assertNoMeshLikePayload(payload, ['baselinePayload']);
}

export function assertAllowedAuthoritativeDeltaPayload(
    payload: Record<string, unknown>,
    ): void {
  assertNoMeshLikePayload(payload, ['deltaPayload']);
}