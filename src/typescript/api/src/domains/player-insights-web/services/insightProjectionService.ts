import {loadPlayerInsightReadModel, type PlayerInsightReadModel} from '../persistence/insightReadRepository.ts';

export function buildPlayerInsightProjection(playerId: string):
    PlayerInsightReadModel {
  return loadPlayerInsightReadModel(playerId);
}
