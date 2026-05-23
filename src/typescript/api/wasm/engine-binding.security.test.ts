import {describe, expect, it} from 'bun:test';

import {resolveTrustedEngineScriptUrl} from './engine-binding.ts';

describe('resolveTrustedEngineScriptUrl security coverage', () => {
  const appOrigin = 'https://banana.engineer';

  it('allows local wasm engine path variants', () => {
    expect(resolveTrustedEngineScriptUrl('/wasm/engine.js', appOrigin))
        .toBe('/wasm/engine.js');
    expect(resolveTrustedEngineScriptUrl(
               '/wasm/engine.js?v=abc123#boot', appOrigin))
        .toBe('/wasm/engine.js?v=abc123#boot');
    expect(resolveTrustedEngineScriptUrl(
               'https://banana.engineer/wasm/engine.js?v=abc123', appOrigin))
        .toBe('/wasm/engine.js?v=abc123');
  });

  it('rejects cross-origin script URLs', () => {
    expect(
        () => resolveTrustedEngineScriptUrl(
            'https://evil.example/wasm/engine.js', appOrigin))
        .toThrow(/same-origin/);
    expect(
        () => resolveTrustedEngineScriptUrl(
            '//evil.example/wasm/engine.js', appOrigin))
        .toThrow(/same-origin/);
  });

  it('rejects non-canonical wasm script paths', () => {
    expect(() => resolveTrustedEngineScriptUrl('/assets/engine.js', appOrigin))
        .toThrow(/\/wasm\/engine\.js/);
    expect(() => resolveTrustedEngineScriptUrl('/wasm/engine.wasm', appOrigin))
        .toThrow(/\/wasm\/engine\.js/);
    expect(() => resolveTrustedEngineScriptUrl('/wasm/../engine.js', appOrigin))
        .toThrow(/\/wasm\/engine\.js/);
  });

  it('rejects credentialed URLs and empty input', () => {
    expect(
        () => resolveTrustedEngineScriptUrl(
            'https://attacker:pw@banana.engineer/wasm/engine.js', appOrigin))
        .toThrow(/credentials/);
    expect(() => resolveTrustedEngineScriptUrl('   ', appOrigin))
        .toThrow(/cannot be empty/);
  });
});
