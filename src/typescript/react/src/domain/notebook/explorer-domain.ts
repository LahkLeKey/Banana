export type MissionRarity = 'common'|'rare'|'epic';

export type MissionSector = {
  readonly file: string; readonly title: string; readonly domain: string; readonly lane: string; readonly code: string; readonly rarity: MissionRarity; readonly threat:
                                                                                                                                                                     number;
};

export type CampaignMissionCard = MissionSector&{
  readonly isSelected: boolean;
};

export type MissionOverlayContract = {
  readonly missionTitle: string; readonly missionBrief: string; readonly objectiveLabel: string; readonly threatLabel:
                                                                                                              string;
};

export function deriveMissionSector(file: string): MissionSector {
  const normalized = file.replace(/\\/g, '/');
  const parts = normalized.split('/').filter(Boolean);
  const lane =
      parts.length > 0 ? parts[parts.length - 1] ?? normalized : normalized;
  const laneLabel = lane.replace(/\.[^.]+$/, '')
                        .replace(/[_-]+/g, ' ')
                        .replace(/\b\w/g, (match) => match.toUpperCase());

  let domain = 'Other';
  if (normalized.includes('/ai/')) {
    domain = 'AI';
  } else if (normalized.includes('/physics/')) {
    domain = 'Physics';
  } else if (normalized.includes('/render/')) {
    domain = 'Render';
  } else if (normalized.includes('/world/')) {
    domain = 'World';
  } else if (normalized.includes('/runtime/')) {
    domain = 'Runtime';
  }

  const checksum = Math.abs(
      normalized.split('').reduce((acc, char) => acc + char.charCodeAt(0), 0));
  const threat = (checksum % 100) + 1;
  const rarity: MissionRarity = threat > 80 ? 'epic' :
      threat > 52                           ? 'rare' :
                                              'common';

  return {
    file,
    title: laneLabel,
    domain,
    lane: laneLabel,
    code: `${domain.slice(0, 2).toUpperCase()}-${checksum % 1000}`,
    rarity,
    threat,
  };
}

export function toCampaignMissionCard(
    file: string, selectedFile: string): CampaignMissionCard {
  const sector = deriveMissionSector(file);
  return {
    ...sector,
    isSelected: file === selectedFile,
  };
}

export function toMissionOverlayContract(card: CampaignMissionCard):
    MissionOverlayContract {
  return {
    missionTitle: card.title,
    missionBrief: `Push into ${card.lane} and secure objective ${
        card.code}. Expect threat level ${card.threat} in this zone.`,
    objectiveLabel: `Objective ID: ${card.code}`,
    threatLabel: `Threat ${card.threat}`,
  };
}
