import {suffix} from 'bun:ffi';
import fs from 'node:fs';
import path from 'node:path';

const DEFAULT_FALLBACK_DIRS = [
  'out/native/bin',
  'out/v3-native/Debug',
  'out/v3-native/Release',
  'out/v3-native-baseline/Debug',
  'build/native/bin',
  'build/native',
];

function expandSearchRoots(cwd: string): string[] {
  const envRoots = (process.env.BANANA_NATIVE_SEARCH_ROOTS ?? '')
                       .split(path.delimiter)
                       .map((value) => value.trim())
                       .filter((value) => value.length > 0);

  return [
    cwd,
    path.resolve(cwd, '..'),
    path.resolve(cwd, '../..'),
    path.resolve(cwd, '../../..'),
    path.resolve(cwd, '../../../..'),
    '/workspace',
    '/app',
    ...envRoots,
  ];
}

function pushFromNativePath(
    target: string[],
    names: readonly string[],
): void {
  const envPath = process.env.BANANA_NATIVE_PATH;
  if (!envPath || envPath.trim().length === 0) {
    return;
  }

  const trimmed = envPath.trim();
  if (trimmed.endsWith(`.${suffix}`)) {
    target.push(trimmed);
    return;
  }

  for (const name of names) {
    target.push(path.join(trimmed, name));
  }
}

function dedupeCandidates(candidates: readonly string[]): string[] {
  const unique = Array.from(new Set(candidates));
  const existing = unique.filter((candidate) => fs.existsSync(candidate));
  const missing = unique.filter((candidate) => !fs.existsSync(candidate));
  return [...existing, ...missing];
}

export type NativeLibraryCandidateOptions = {
  fallbackDirs?: readonly string[];
};

export function resolveBananaNativeLibraryCandidates(
    options: NativeLibraryCandidateOptions = {},
): string[] {
  const names = [`libbanana_native.${suffix}`, `banana_native.${suffix}`];
  const cwd = process.cwd();
  const fallbackDirs = options.fallbackDirs ?? DEFAULT_FALLBACK_DIRS;
  const roots = expandSearchRoots(cwd);
  const candidates: string[] = [];

  pushFromNativePath(candidates, names);

  for (const root of roots) {
    for (const name of names) {
      candidates.push(path.join(root, name));
      for (const dir of fallbackDirs) {
        candidates.push(path.join(root, dir, name));
      }
    }
  }

  return dedupeCandidates(candidates);
}
