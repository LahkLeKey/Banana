import {createK3h4ApplicationService, type K3h4ApplicationService,} from '../../application/k3h4/k3h4ApplicationService.ts';
import {createNativeK3h4AuthoritativeAnalyticsAdapter,} from '../../infrastructure/k3h4/nativeK3h4AuthoritativeAnalyticsAdapter.ts';
import type {NativeNetcodeService} from '../../services/nativeNetcode.ts';

export function composeK3h4ApplicationService(
    netcode: NativeNetcodeService,
    ): K3h4ApplicationService {
  const analyticsPort = createNativeK3h4AuthoritativeAnalyticsAdapter(netcode);
  return createK3h4ApplicationService(analyticsPort);
}
