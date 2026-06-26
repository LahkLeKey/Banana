import {FFIType, type Pointer, ptr} from 'bun:ffi';

import {loadBananaNativeSymbols,} from '../lib/native-interop-loader.ts';

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
      connectionUriPtr: Pointer,
      poolModePtr: Pointer,
      defaultPoolSize: number,
      ) => number;
  banana_native_v3_pgbouncer_health_json:
      (bufferPtr: Pointer, bufferLen: number) => number;
};

function cStringPtr(value: string): Pointer {
  return ptr(Buffer.from(`${value}\0`, 'utf8'));
}

function createNativeBinding(): NativePgBouncerSymbols {
  return loadBananaNativeSymbols<NativePgBouncerSymbols>(
      {
        banana_native_v3_pgbouncer_available: {
          args: [],
          returns: FFIType.i32,
        },
        banana_native_v3_pgbouncer_configure: {
          args: [FFIType.ptr, FFIType.ptr, FFIType.i32],
          returns: FFIType.i32,
        },
        banana_native_v3_pgbouncer_health_json: {
          args: [FFIType.ptr, FFIType.i32],
          returns: FFIType.i32,
        },
      },
      'PgBouncer bridge',
      {
        fallbackDirs: [
          'out/native/bin',
          'out/v3-native/Debug',
          'out/v3-native/Release',
          'build/native/bin',
          'build/native',
        ],
      },
  );
}

export class NativePgBouncerBridge {
  private readonly symbols = createNativeBinding();

  public isAvailable(): boolean {
    return this.symbols.banana_native_v3_pgbouncer_available() === 1;
  }

  public configure(config: NativePgBouncerConfig): void {
    const result = this.symbols.banana_native_v3_pgbouncer_configure(
        cStringPtr(config.connectionUri),
        cStringPtr(config.poolMode ?? 'transaction'),
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
