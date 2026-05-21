import {dlopen, FFIType, type Pointer, ptr, suffix} from 'bun:ffi';
import path from 'node:path';

export type NativePgBouncerConfig = {
  connectionUri: string;
  poolMode?: string;
  defaultPoolSize?: number;
};

export type NativePgBouncerHealth = {
  available: boolean; enabled: boolean;
  poolMode?: string;
  defaultPoolSize?: number;
  reason?: string;
};

type NativePgBouncerSymbols = {
  banana_native_v3_pgbouncer_available: () => number;
  banana_native_v3_pgbouncer_configure: (
      connectionUri: string,
      poolMode: string,
      defaultPoolSize: number,
      ) => number;
  banana_native_v3_pgbouncer_health_json:
      (bufferPtr: Pointer, bufferLen: number) => number;
};

function resolveNativeLibraryCandidates(): string[] {
  const ext = suffix;
  const names = [`libbanana_native.${ext}`, `banana_native.${ext}`];
  const envPath = process.env.BANANA_NATIVE_PATH;
  const candidates: string[] = [];

  if (envPath && envPath.trim().length > 0) {
    const trimmed = envPath.trim();
    if (trimmed.endsWith(`.${ext}`)) {
      candidates.push(trimmed);
    } else {
      for (const name of names) {
        candidates.push(path.join(trimmed, name));
      }
    }
  }

  const repoRoot = path.resolve(process.cwd(), '../../..');
  const fallbackDirs = [
    'out/native/bin',
    'out/v3-native/Debug',
    'out/v3-native/Release',
    'build/native/bin',
    'build/native',
  ];

  for (const name of names) {
    for (const dir of fallbackDirs) {
      candidates.push(path.join(repoRoot, dir, name));
    }
  }

  return Array.from(new Set(candidates));
}

function createNativeBinding(): NativePgBouncerSymbols {
  const candidates = resolveNativeLibraryCandidates();
  let lastError: unknown = null;

  for (const candidate of candidates) {
    try {
      const library = dlopen(candidate, {
        banana_native_v3_pgbouncer_available: {
          args: [],
          returns: FFIType.i32,
        },
        banana_native_v3_pgbouncer_configure: {
          args: [FFIType.cstring, FFIType.cstring, FFIType.i32],
          returns: FFIType.i32,
        },
        banana_native_v3_pgbouncer_health_json: {
          args: [FFIType.ptr, FFIType.i32],
          returns: FFIType.i32,
        },
      });

      return library.symbols as unknown as NativePgBouncerSymbols;
    } catch (error) {
      lastError = error;
    }
  }

  throw new Error(
      `Unable to load Banana native library for PgBouncer bridge. Candidates: ${
          candidates.join(', ')}. Last error: ${String(lastError)}`,
  );
}

export class NativePgBouncerBridge {
  private readonly symbols = createNativeBinding();

  public isAvailable(): boolean {
    return this.symbols.banana_native_v3_pgbouncer_available() === 1;
  }

  public configure(config: NativePgBouncerConfig): void {
    const result = this.symbols.banana_native_v3_pgbouncer_configure(
        config.connectionUri,
        config.poolMode ?? 'transaction',
        config.defaultPoolSize ?? 20,
    );

    if (result !== 0) {
      throw new Error(
          `Native PgBouncer configure failed with status ${result}`);
    }
  }

  public health(): NativePgBouncerHealth {
    const buffer = new Uint8Array(2048);
    const rc = this.symbols.banana_native_v3_pgbouncer_health_json(
        ptr(buffer), buffer.length);
    if (rc !== 0) {
      throw new Error(`Native PgBouncer health probe failed with status ${rc}`);
    }

    const nulIndex = buffer.indexOf(0);
    const slice = nulIndex >= 0 ? buffer.slice(0, nulIndex) : buffer;
    const json = new TextDecoder().decode(slice);
    return JSON.parse(json) as NativePgBouncerHealth;
  }
}

export function createDefaultNativePgBouncerBridge(): NativePgBouncerBridge {
  return new NativePgBouncerBridge();
}
