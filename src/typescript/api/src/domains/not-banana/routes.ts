import type {FastifyInstance} from 'fastify';

import type {NotBananaJunkClassification, NotBananaJunkRequest, NotBananaJunkResponse, NotBananaPolymorphicActor, NotBananaPolymorphicEntity,} from '../../contracts/http.js';
import {logDomainCall, saveNotBananaClassification} from '../../shared/prisma.js';

const BANANA_SIGNAL_TOKENS = new Set([
  'banana',
  'bananas',
  'plantain',
  'plantains',
  'cavendish',
  'ripeness',
  'ripe',
  'unripe',
  'peel',
  'fruit',
  'ethylene',
  'bunch',
  'yellow',
]);

type NormalizedNotBananaRequest = {
  actors: NotBananaPolymorphicActor[]; entities: NotBananaPolymorphicEntity[];
  actorPayloads: unknown[];
  entityPayloads: unknown[];
  junk: unknown;
  metadata: Record<string, unknown>| undefined;
};

function isRecord(value: unknown): value is Record<string, unknown> {
  return !!value && typeof value === 'object' && !Array.isArray(value);
}

function tokeniseString(value: string): string[] {
  return value.toLowerCase().match(/[a-z0-9]+/g) ?? [];
}

function extractStringField(
    record: Record<string, unknown>, keys: string[]): string|undefined {
  for (const key of keys) {
    const value = record[key];
    if (typeof value === 'string' && value.trim().length > 0) {
      return value.trim();
    }
  }

  return undefined;
}

function clamp(value: number, min: number, max: number): number {
  return Math.min(max, Math.max(min, value));
}

function roundProbability(value: number): number {
  return Math.round(value * 10000) / 10000;
}

function normalizeActor(value: unknown):
    {normalized: NotBananaPolymorphicActor; payload: unknown;} {
  if (!isRecord(value)) {
    throw new Error('actor entries must be JSON objects.');
  }

  const actorType = extractStringField(value, ['actorType', 'type', 'kind']);
  if (!actorType) {
    throw new Error(
        'actor entries must include actorType/type/kind discriminator.');
  }

  const actorId =
      extractStringField(value, ['actorId', 'id', 'actorKey']) ?? null;
  return {
    normalized: {actorType, actorId},
    payload: value,
  };
}

function normalizeEntity(value: unknown):
    {normalized: NotBananaPolymorphicEntity; payload: unknown;} {
  if (!isRecord(value)) {
    throw new Error('entity entries must be JSON objects.');
  }

  const entityType = extractStringField(value, ['entityType', 'type', 'kind']);
  if (!entityType) {
    throw new Error(
        'entity entries must include entityType/type/kind discriminator.');
  }

  const entityId =
      extractStringField(value, ['entityId', 'id', 'entityKey']) ?? null;
  return {
    normalized: {entityType, entityId},
    payload: value,
  };
}

function stripKnownPolymorphicFields(value: Record<string, unknown>):
    Record<string, unknown> {
  const junkCandidate = {...value};
  delete junkCandidate.actors;
  delete junkCandidate.entities;
  delete junkCandidate.actor;
  delete junkCandidate.entity;
  delete junkCandidate.junk;
  delete junkCandidate.metadata;
  return junkCandidate;
}

function normalizeRequest(payload: unknown): NormalizedNotBananaRequest {
  if (!isRecord(payload)) {
    throw new Error('not-banana payload must be a JSON object.');
  }

  const requestBody = payload as NotBananaJunkRequest;

  const actorInputs: unknown[] = [];
  if (Array.isArray(requestBody.actors)) {
    actorInputs.push(...requestBody.actors);
  }
  if (requestBody.actor !== undefined) {
    actorInputs.push(requestBody.actor);
  }

  const entityInputs: unknown[] = [];
  if (Array.isArray(requestBody.entities)) {
    entityInputs.push(...requestBody.entities);
  }
  if (requestBody.entity !== undefined) {
    entityInputs.push(requestBody.entity);
  }

  const actors = actorInputs.map(normalizeActor);
  const entities = entityInputs.map(normalizeEntity);

  const fallbackJunk = stripKnownPolymorphicFields(payload);
  const fallbackHasValues = Object.keys(fallbackJunk).length > 0;
  const junk = requestBody.junk ?? (fallbackHasValues ? fallbackJunk : null);

  if (actors.length === 0 && entities.length === 0 && junk === null) {
    throw new Error(
        'payload must include actors, entities, or arbitrary junk content.');
  }

  const metadata =
      isRecord(requestBody.metadata) ? requestBody.metadata : undefined;

  return {
    actors: actors.map((entry) => entry.normalized),
    entities: entities.map((entry) => entry.normalized),
    actorPayloads: actors.map((entry) => entry.payload),
    entityPayloads: entities.map((entry) => entry.payload),
    junk,
    metadata,
  };
}

function collectTokens(value: unknown, tokens: Set<string>, depth = 0): void {
  if (depth > 5 || value === null || value === undefined) {
    return;
  }

  if (typeof value === 'string') {
    for (const token of tokeniseString(value)) {
      tokens.add(token);
    }
    return;
  }

  if (typeof value === 'number' || typeof value === 'boolean') {
    tokens.add(String(value).toLowerCase());
    return;
  }

  if (Array.isArray(value)) {
    for (const entry of value.slice(0, 128)) {
      collectTokens(entry, tokens, depth + 1);
    }
    return;
  }

  if (!isRecord(value)) {
    return;
  }

  const entries = Object.entries(value).slice(0, 128);
  for (const [key, entryValue] of entries) {
    for (const token of tokeniseString(key)) {
      tokens.add(token);
    }
    collectTokens(entryValue, tokens, depth + 1);
  }
}

function classifyNotBanana(normalized: NormalizedNotBananaRequest):
    NotBananaJunkResponse {
  const tokens = new Set<string>();

  collectTokens(normalized.actors, tokens);
  collectTokens(normalized.entities, tokens);
  collectTokens(normalized.actorPayloads, tokens);
  collectTokens(normalized.entityPayloads, tokens);
  collectTokens(normalized.junk, tokens);
  collectTokens(normalized.metadata, tokens);

  let bananaTokenHits = 0;
  for (const token of tokens) {
    if (BANANA_SIGNAL_TOKENS.has(token)) {
      bananaTokenHits += 1;
    }
  }

  const tokenCount = Math.max(tokens.size, 1);
  const bananaSignal = bananaTokenHits / tokenCount;
  const polymorphicWeight = Math.min(
      0.35, (normalized.actors.length + normalized.entities.length) * 0.04);

  const bananaProbability = roundProbability(
      clamp(0.04 + (bananaSignal * 0.9) - polymorphicWeight, 0.01, 0.99));
  const notBananaProbability =
      roundProbability(clamp(1 - bananaProbability, 0.01, 0.99));
  const junkConfidence = roundProbability(clamp(
      notBananaProbability + Math.min(0.2, tokenCount * 0.004), 0.05, 0.99));

  const classification: NotBananaJunkClassification =
      notBananaProbability >= 0.5 ? 'NOT_BANANA' : 'MAYBE_BANANA';

  const message = classification === 'NOT_BANANA' ?
      'object set classified as non-banana junk using polymorphic actor/entity analysis.' :
      'object set contains banana-like signals; manual review is recommended.';

  return {
    classification,
    bananaProbability,
    notBananaProbability,
    junkConfidence,
    actorCount: normalized.actors.length,
    entityCount: normalized.entities.length,
    normalizedActors: normalized.actors,
    normalizedEntities: normalized.entities,
    message,
  };
}

export function registerNotBananaRoutes(app: FastifyInstance): void {
  app.post('/not-banana/junk', async (request, reply) => {
    const route = '/not-banana/junk';

    let normalized: NormalizedNotBananaRequest;
    try {
      normalized = normalizeRequest(request.body);
    } catch (error) {
      const payload = {
        message: error instanceof Error ?
            error.message :
            'invalid not-banana polymorphic payload.'
      };
      await logDomainCall('not-banana', route, 400, request.body, payload);
      return reply.code(400).send(payload);
    }

    try {
      const response = classifyNotBanana(normalized);
      await saveNotBananaClassification({
        classification: response.classification,
        bananaProbability: response.bananaProbability,
        notBananaProbability: response.notBananaProbability,
        junkConfidence: response.junkConfidence,
        actorCount: response.actorCount,
        entityCount: response.entityCount,
        requestBody: request.body,
        responseBody: response,
        source: 'heuristic-polymorphic-v1',
      });
      await logDomainCall('not-banana', route, 200, request.body, response);
      return reply.code(200).send(response);
    } catch (error) {
      request.log.error({err: error}, 'not-banana junk classification failed');
      const payload = {message: 'not-banana classification failed.'};
      await logDomainCall('not-banana', route, 500, request.body, payload);
      return reply.code(500).send(payload);
    }
  });
}
