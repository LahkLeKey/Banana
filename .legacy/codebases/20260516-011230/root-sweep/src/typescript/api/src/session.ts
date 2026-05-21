export type SessionId = string;

export type SessionState = {
    id: SessionId; status : "active" | "ended";
};

const sessions = new Map<SessionId, SessionState>();

export function startSession(id: SessionId): SessionState
{
    const state: SessionState = {id, status : "active"};
    sessions.set(id, state);
    return state;
}

export function rejoinSession(id: SessionId): SessionState|null
{
    return sessions.get(id) ?? null;
}

export function endSession(id: SessionId): SessionState|null
{
    const existing = sessions.get(id);
    if (!existing)
    {
        return null;
    }
    const ended: SessionState = {...existing, status : "ended"};
    sessions.set(id, ended);
    return ended;
}

export function apiHealth(): {ok: true; surface : "api"}
{
    return {ok : true, surface : "api"};
}
