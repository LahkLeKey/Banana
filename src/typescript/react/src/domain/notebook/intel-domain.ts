import {normalizeDependencyLabel} from './mission-domain';

export type IntelSeverity = 'low'|'medium'|'high';

export type IntelEvent = {
  readonly id: string; readonly label: string; readonly severity: IntelSeverity; readonly detail:
                                                                                              string;
};

export type IntelSignals = {
  readonly lineCount: number; readonly functionCount: number; readonly functionNames: string[]; readonly includeCount: number; readonly includeTargets: string[]; readonly primaryDependencyLabel: string; readonly ifCount: number; readonly loopCount: number; readonly pointerCount: number; readonly callDensity: number; readonly branchPressure: number; readonly memoryRisk: number; readonly dependencyPulse: number; readonly hotspotScore: number; readonly riskLabel: 'Critical' | 'Elevated' | 'Guarded' | 'Stable'; readonly sourcePreviewWithLineNumbers:
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              string;
};

function clamp(value: number, max: number): number {
  return Math.min(max, value);
}

export function buildIntelSignals(content: string): IntelSignals {
  const lines = content.split('\n');
  const lineCount = lines.length;
  const functionCount = (content.match(/\b[a-zA-Z_]\w*\s*\(/g) ?? []).length;

  const functionNames =
      Array
          .from(new Set(
              (content.match(/\b([a-zA-Z_]\w*)\s*\(/g) ?? [])
                  .map((match) => match.replace('(', '').trim())
                  .filter(
                      (name) =>
                          !['if', 'for', 'while', 'switch', 'return', 'sizeof']
                               .includes(name)),
              ))
          .slice(0, 3);

  const includeTargets =
      (content.match(/^\s*#include\s+[<"].+[>"]/gm) ?? [])
          .slice(0, 3)
          .map((entry) => entry.replace(/^\s*#include\s+/, ''));

  const includeCount = (content.match(/^\s*#include\b/gm) ?? []).length;
  const ifCount = (content.match(/\bif\s*\(/g) ?? []).length;
  const loopCount = (content.match(/\b(for|while)\s*\(/g) ?? []).length;
  const pointerCount = (content.match(/\*/g) ?? []).length;

  const callDensity =
      lineCount <= 0 ? 0 : Math.round((functionCount / lineCount) * 100);
  const branchPressure = clamp(ifCount * 6 + loopCount * 9, 100);
  const memoryRisk = clamp(pointerCount, 100);
  const dependencyPulse = clamp(includeCount * 14, 100);
  const hotspotScore = clamp(
      Math.round(
          (branchPressure * 0.45) + (memoryRisk * 0.35) +
          (dependencyPulse * 0.2)),
      100);

  const riskLabel: IntelSignals['riskLabel'] = hotspotScore >= 75 ? 'Critical' :
      hotspotScore >= 50                                          ? 'Elevated' :
      hotspotScore >= 25                                          ? 'Guarded' :
                                                                    'Stable';

  return {
    lineCount,
    functionCount,
    functionNames,
    includeCount,
    includeTargets,
    primaryDependencyLabel: includeTargets[0] ?
        normalizeDependencyLabel(includeTargets[0]) :
        'No dependency route detected',
    ifCount,
    loopCount,
    pointerCount,
    callDensity,
    branchPressure,
    memoryRisk,
    dependencyPulse,
    hotspotScore,
    riskLabel,
    sourcePreviewWithLineNumbers:
        lines.slice(0, 80)
            .map(
                (line, index) =>
                    `${String(index + 1).padStart(3, '0')} | ${line}`)
            .join('\n'),
  };
}

export function buildMissionIntelEvents(signals: IntelSignals): IntelEvent[] {
  return [
    {
      id: 'threat',
      label: signals.branchPressure >= 70 ? 'Threat Surge' :
          signals.branchPressure >= 40    ? 'Tactical Tension' :
                                            'Stable Frontline',
      severity: signals.branchPressure >= 70 ? 'high' :
          signals.branchPressure >= 40       ? 'medium' :
                                               'low',
      detail: `Hostile pressure ${signals.branchPressure}% across ${
          signals.ifCount} choke points and ${
          signals.loopCount} active patrol loops.`,
    },
    {
      id: 'memory',
      label: signals.memoryRisk >= 60 ? 'Supply Fracture Risk' :
          signals.memoryRisk >= 30    ? 'Supply Watch' :
                                        'Supply Secure',
      severity: signals.memoryRisk >= 60 ? 'high' :
          signals.memoryRisk >= 30       ? 'medium' :
                                           'low',
      detail: `${signals.pointerCount} supply vectors are active in this lane.`,
    },
    {
      id: 'dependency',
      label: signals.dependencyPulse >= 60 ? 'Logistics Spike' :
          signals.dependencyPulse >= 30    ? 'Logistics Drift' :
                                             'Logistics Calm',
      severity: signals.dependencyPulse >= 60 ? 'high' :
          signals.dependencyPulse >= 30       ? 'medium' :
                                                'low',
      detail: `${signals.primaryDependencyLabel} anchors ${
          signals.includeCount} support channels.`,
    },
    {
      id: 'signal',
      label: signals.callDensity >= 22 ? 'High Signal Traffic' :
          signals.callDensity >= 10    ? 'Moderate Signal Traffic' :
                                         'Low Signal Traffic',
      severity: signals.callDensity >= 22 ? 'high' :
          signals.callDensity >= 10       ? 'medium' :
                                            'low',
      detail: `${signals.functionNames[0] ?? 'Primary relay unknown'} leads ${
          signals.functionCount} active command signals.`,
    },
  ];
}
