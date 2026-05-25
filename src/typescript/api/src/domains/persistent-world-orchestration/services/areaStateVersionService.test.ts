import {beforeEach, describe, expect, test} from 'bun:test';

import {createAreaStateVersionService, resetAreaStateVersionServiceForTests} from './areaStateVersionService.ts';

describe('area state version service', () => {
  beforeEach(() => {
    resetAreaStateVersionServiceForTests();
  });

  test('increments monotonically and rejects stale base versions', () => {
    const service = createAreaStateVersionService();
    service.ensure('area-a', 'sig0');

    const next = service.advance('area-a', 1, 'sig1');
    expect(next.areaStateVersion).toBe(1);

    expect(() => service.assertNotStale('area-a', 0))
        .toThrow('persistent_world_stale_delta_version');
    expect(() => service.assertNotStale('area-a', 1)).not.toThrow();
  });
});
