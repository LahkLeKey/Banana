import type {WorldService} from './world.ts';

export function createWorldServiceForTests(): WorldService {
  return {
    async getChunkBinary(chunkX: number, chunkZ: number): Promise<Buffer> {
      const buffer = Buffer.alloc(32);
      const view =
          new DataView(buffer.buffer, buffer.byteOffset, buffer.byteLength);
      view.setInt32(0, chunkX, true);
      view.setInt32(4, chunkZ, true);
      view.setUint32(8, 1, true);
      return buffer;
    },
  };
}
