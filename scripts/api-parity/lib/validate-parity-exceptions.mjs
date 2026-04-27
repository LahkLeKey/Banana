#!/usr/bin/env node

import {readFileSync} from 'node:fs';

const REQUIRED_FIELDS = [
  'route_key',
  'mismatch_type',
  'owner',
  'rationale',
  'approved_at',
  'expires_at',
];

function parseJson(filePath) {
  return JSON.parse(readFileSync(filePath, 'utf8'));
}

export function validateParityExceptions(filePath, now = new Date()) {
  const payload = parseJson(filePath);
  const exceptions = Array.isArray(payload.exceptions) ? payload.exceptions : [];
  const errors = [];

  for (const [index, entry] of exceptions.entries()) {
    for (const field of REQUIRED_FIELDS) {
      if (!(field in entry) || String(entry[field]).trim().length === 0) {
        errors.push(`exceptions[${index}] missing required field '${field}'`);
      }
    }

    if (entry.expires_at) {
      const expiresAt = new Date(entry.expires_at);
      if (Number.isNaN(expiresAt.getTime())) {
        errors.push(`exceptions[${index}] has invalid expires_at '${entry.expires_at}'`);
      } else if (expiresAt.getTime() <= now.getTime()) {
        errors.push(`exceptions[${index}] expired at '${entry.expires_at}'`);
      }
    }
  }

  return {
    ok: errors.length === 0,
    errors,
    exceptions,
  };
}
