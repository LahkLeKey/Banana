export interface DeltaUpdate {
    readonly entityId: string;
    readonly tick: number;
    readonly changes: Record<string, unknown>;
    readonly timestamp: number;
}

export interface WorldState {
    readonly tick: number;
    readonly timestamp: number;
    readonly entities: Record<string, Record<string, unknown>>;
    readonly isAuthoritative: boolean;
}

export interface ReplicatedState {
    tick: number;
    state: WorldState;
}

export interface ReplicationDomainConfig {
    maxHistory: number;
}

export class ReplicationDomain
{
    private readonly config: ReplicationDomainConfig;
    private stateHistory: ReplicatedState[] = [];

    constructor(config: ReplicationDomainConfig)
    {
        this.config = config;
    }

    public recordState(tick: number, state: WorldState): void
    {
        this.stateHistory.push({tick, state});
        if (this.stateHistory.length > this.config.maxHistory)
        {
            this.stateHistory.shift();
        }
    }

    public applyDelta(delta: DeltaUpdate): WorldState
    {
        const latest = this.getLatestState()?.state;
        const baseEntities = latest?.entities ?? {};
        const entity = baseEntities[delta.entityId] ?? {};
        const mergedEntity = {...entity, ...delta.changes};
        const nextState: WorldState = {
            tick : delta.tick,
            timestamp : delta.timestamp,
            entities : {...baseEntities, [delta.entityId] : mergedEntity},
            isAuthoritative : true,
        };

        this.recordState(delta.tick, nextState);
        return nextState;
    }

    public getStateAtTick(tick: number): ReplicatedState|undefined
    {
        return this.stateHistory.find(s => s.tick === tick);
    }

    public getLatestState(): ReplicatedState|undefined
    {
        return this.stateHistory[this.stateHistory.length - 1];
    }

    public getHistory(): ReplicatedState[]
    {
        return this.stateHistory;
    }
}
