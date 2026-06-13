import {describe, expect, it} from 'bun:test';

import {buildContractK3h4ProjectionModel, buildContractNodeVectorModel, buildNodeLinkConfidenceModel, buildRewardSignalModel,} from './network-domain';

describe('network-domain production fallback guard', () => {
  it('keeps deterministic helper exports available for non-production tooling',
     () => {
       expect(typeof buildRewardSignalModel).toBe('function');
       expect(typeof buildNodeLinkConfidenceModel).toBe('function');
       expect(typeof buildContractNodeVectorModel).toBe('function');
       expect(typeof buildContractK3h4ProjectionModel).toBe('function');
     });

  it('proves session path does not call local recompute helpers in production flow',
     async () => {
       const sourceUrl = new URL('./useNetcodeSession.ts', import.meta.url);
       const source = await Bun.file(sourceUrl).text();

       expect(source.includes('fetchNetcodeAnalytics(')).toBe(true);
       expect(source.includes('buildRewardSignalModel(')).toBe(false);
       expect(source.includes('buildNodeLinkConfidenceModel(')).toBe(false);
       expect(source.includes('buildContractNodeVectorModel(')).toBe(false);
       expect(source.includes('buildContractK3h4ProjectionModel(')).toBe(false);
     });
});
