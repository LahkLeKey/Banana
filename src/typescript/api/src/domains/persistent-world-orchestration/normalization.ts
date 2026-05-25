import {createHash, randomUUID} from 'node:crypto';

import {type AreaIdentity, AreaIdentitySchema, type GenerationFingerprint, type RevisitBaselineRequest, RevisitBaselineRequestSchema,} from '../../lib/contracts/v1/persistentWorld.ts';

export const DETERMINISTIC_NORMALIZATION_SCHEMA_VERSION = 'v1';
export const DETERMINISTIC_DERIVATION_POLICY_VERSION = 'v1';

function canonicalIdentityTuple(input: RevisitBaselineRequest): string {
  return [
    input.worldId.trim().toLowerCase(),
    input.laneId.trim(),
    String(input.chunkX),
    String(input.chunkY),
    String(input.partitionEpoch),
  ].join(':');
}

function hashSha256(payload: string): string {
  return createHash('sha256').update(payload).digest('hex');
}

export function normalizeRevisitBaselineRequest(raw: unknown):
    RevisitBaselineRequest {
  const parsed = RevisitBaselineRequestSchema.parse(raw);
  return {
    worldId: parsed.worldId.trim().toLowerCase(),
    laneId: parsed.laneId.trim(),
    chunkX: parsed.chunkX,
    chunkY: parsed.chunkY,
    partitionEpoch: parsed.partitionEpoch,
  };
}

export function buildAreaIdentity(input: RevisitBaselineRequest): AreaIdentity {
  const tuple = canonicalIdentityTuple(input);
  const hash = hashSha256(tuple);
  const areaId = `area_${hash.slice(0, 24)}`;

  return AreaIdentitySchema.parse({
    worldId: input.worldId,
    laneId: input.laneId,
    chunkX: input.chunkX,
    chunkY: input.chunkY,
    partitionEpoch: input.partitionEpoch,
    identityHash: hash,
    areaId,
  });
}

export function buildGenerationFingerprint(input: RevisitBaselineRequest):
    GenerationFingerprint {
  const canonicalPayload = JSON.stringify({
    worldId: input.worldId,
    laneId: input.laneId,
    chunkX: input.chunkX,
    chunkY: input.chunkY,
    partitionEpoch: input.partitionEpoch,
    derivationPolicyVersion: DETERMINISTIC_DERIVATION_POLICY_VERSION,
    normalizationSchemaVersion: DETERMINISTIC_NORMALIZATION_SCHEMA_VERSION,
  });

  return {
    derivationPolicyVersion: DETERMINISTIC_DERIVATION_POLICY_VERSION,
    normalizationSchemaVersion: DETERMINISTIC_NORMALIZATION_SCHEMA_VERSION,
    generationInputFingerprint: hashSha256(canonicalPayload),
  };
}

export function makeDeterministicBaselineId(identityHash: string): string {
  const source = hashSha256(`baseline:${identityHash}`);
  const hex = source.slice(0, 32).split('');
  hex[12] = '4';
  const variant = parseInt(hex[16] ?? '0', 16);
  hex[16] = ((variant & 0x3) | 0x8).toString(16);
  const part1 = hex.slice(0, 8).join('');
  const part2 = hex.slice(8, 12).join('');
  const part3 = hex.slice(12, 16).join('');
  const part4 = hex.slice(16, 20).join('');
  const part5 = hex.slice(20, 32).join('');
  return `${part1}-${part2}-${part3}-${part4}-${part5}`;
}

export function makeDeterministicCanonicalStateSignature(identityHash: string):
    string {
  return hashSha256(`canonical:${identityHash}`);
}

export function makeRequestCorrelationId(): string {
  return randomUUID();
}
