type DomainOwner =|'gameplay-session-orchestration'|'player-identity-account'|
    'progression-inventory'|'player-insights-web'|'legacy';

const ROUTE_OWNER_RULES: Array<{prefix: string; owner: DomainOwner}> = [
  {prefix: '/v1/gameplay', owner: 'gameplay-session-orchestration'},
  {prefix: '/v1/player/insights', owner: 'player-insights-web'},
  {prefix: '/v1/player/progression', owner: 'progression-inventory'},
  {prefix: '/v1/player/inventory', owner: 'progression-inventory'},
  {prefix: '/v1/player/account', owner: 'player-identity-account'},
  {prefix: '/v1/player', owner: 'player-identity-account'},
  {prefix: '/api/game/session', owner: 'legacy'},
  {prefix: '/world', owner: 'legacy'},
  {prefix: '/auth', owner: 'legacy'},
  {prefix: '/health', owner: 'legacy'},
  {prefix: '/chat', owner: 'legacy'},
];

export function resolveRouteOwner(routePath: string): DomainOwner {
  const found =
      ROUTE_OWNER_RULES.find((rule) => routePath.startsWith(rule.prefix));
  if (!found) {
    return 'legacy';
  }
  return found.owner;
}

export function assertRouteOwnershipCoverage(routeInventoryText: string): void {
  const hasGameplay = routeInventoryText.includes('/v1/gameplay');
  const hasPlayer = routeInventoryText.includes('/v1/player');
  if (!hasGameplay || !hasPlayer) {
    throw new Error(
        'Feature 007 ownership guard failed: /v1/gameplay and /v1/player routes must be registered.');
  }
}
