import type {K3h4AuthoritativeAnalyticsPort} from '../../domain/k3h4/k3h4AuthoritativeAnalytics.ts';
import type {NativeNetcodeService} from '../../services/nativeNetcode.ts';
import {createNetcodeAnalyticsAuthoritativeComputeOrchestrator,} from '../../services/netcodeAuthoritativeComputeOrchestrator.ts';

/** Bridges the domain port onto the native-backed compute orchestrator. */
export function createNativeK3h4AuthoritativeAnalyticsAdapter(
    netcode: NativeNetcodeService,
    ): K3h4AuthoritativeAnalyticsPort {
  return createNetcodeAnalyticsAuthoritativeComputeOrchestrator(netcode);
}
