#include "banana_anticheat.h"
#include "banana_status.h"

#include <string.h>

#define BANANA_ANTICHEAT_MAX_SESSIONS 256
#define BANANA_ANTICHEAT_SESSION_ID_MAX 64
#define BANANA_ANTICHEAT_FLAG_THRESHOLD 8

typedef struct BananaAnticheatSession
{
    int in_use;
    char session_id[BANANA_ANTICHEAT_SESSION_ID_MAX];
    uint32_t last_usermode_heartbeat;
    uint32_t last_driver_heartbeat;
    uint32_t last_simulation_tick;
    int score;
    uint32_t rolling_integrity_hash;
} BananaAnticheatSession;

static BananaAnticheatSession s_sessions[BANANA_ANTICHEAT_MAX_SESSIONS];

static uint32_t anticheat_hash_str(const char *text)
{
    uint32_t hash = 2166136261u;
    while (*text)
    {
        hash ^= (unsigned char)(*text);
        hash *= 16777619u;
        text++;
    }
    return hash;
}

static uint32_t anticheat_mix_u32(uint32_t hash, uint32_t value)
{
    hash ^= value;
    hash *= 16777619u;
    return hash;
}

static BananaAnticheatSession *find_session(const char *session_id, int create)
{
    uint32_t start = anticheat_hash_str(session_id) % BANANA_ANTICHEAT_MAX_SESSIONS;

    for (uint32_t offset = 0; offset < BANANA_ANTICHEAT_MAX_SESSIONS; offset++)
    {
        uint32_t idx = (start + offset) % BANANA_ANTICHEAT_MAX_SESSIONS;
        BananaAnticheatSession *slot = &s_sessions[idx];

        if (slot->in_use && strncmp(slot->session_id, session_id,
                                    BANANA_ANTICHEAT_SESSION_ID_MAX - 1) == 0)
            return slot;

        if (!slot->in_use)
        {
            if (!create)
                return NULL;

            memset(slot, 0, sizeof(*slot));
            slot->in_use = 1;
            strncpy(slot->session_id, session_id, BANANA_ANTICHEAT_SESSION_ID_MAX - 1);
            slot->rolling_integrity_hash = anticheat_hash_str(session_id);
            return slot;
        }
    }

    return NULL;
}

static int compute_flagged(const BananaAnticheatSession *session)
{
    return session->score >= BANANA_ANTICHEAT_FLAG_THRESHOLD ? 1 : 0;
}

int banana_core_anticheat_reset_session(const char *session_id)
{
    BananaAnticheatSession *session = NULL;

    if (!session_id || !session_id[0])
        return BANANA_INVALID_ARGUMENT;

    session = find_session(session_id, 1);
    if (!session)
        return BANANA_INTERNAL_ERROR;

    memset(session, 0, sizeof(*session));
    session->in_use = 1;
    strncpy(session->session_id, session_id, BANANA_ANTICHEAT_SESSION_ID_MAX - 1);
    session->rolling_integrity_hash = anticheat_hash_str(session_id);
    return BANANA_OK;
}

int banana_core_anticheat_submit_usermode_heartbeat(const char *session_id,
                                                     uint32_t heartbeat_sequence,
                                                     uint32_t simulation_tick,
                                                     int debugger_present,
                                                     int code_integrity_violations,
                                                     int memory_map_anomalies,
                                                     int timing_anomalies,
                                                     int *out_score,
                                                     int *out_flagged)
{
    BananaAnticheatSession *session = NULL;
    int delta = 0;

    if (!session_id || !session_id[0] || !out_score || !out_flagged)
        return BANANA_INVALID_ARGUMENT;

    if (code_integrity_violations < 0 || memory_map_anomalies < 0 || timing_anomalies < 0)
        return BANANA_INVALID_ARGUMENT;

    session = find_session(session_id, 1);
    if (!session)
        return BANANA_INTERNAL_ERROR;

    if (heartbeat_sequence <= session->last_usermode_heartbeat)
        delta += 2;
    else if (session->last_usermode_heartbeat > 0 &&
             heartbeat_sequence - session->last_usermode_heartbeat > 8)
        delta += 1;

    if (simulation_tick + 2 < session->last_simulation_tick)
        delta += 2;

    if (debugger_present)
        delta += 2;
    delta += code_integrity_violations * 3;
    delta += memory_map_anomalies * 2;
    delta += timing_anomalies;

    if (delta == 0 && session->score > 0)
        session->score -= 1;
    else
        session->score += delta;

    if (session->score < 0)
        session->score = 0;

    if (heartbeat_sequence > session->last_usermode_heartbeat)
        session->last_usermode_heartbeat = heartbeat_sequence;
    if (simulation_tick > session->last_simulation_tick)
        session->last_simulation_tick = simulation_tick;

    session->rolling_integrity_hash = anticheat_mix_u32(session->rolling_integrity_hash,
                                                        heartbeat_sequence);
    session->rolling_integrity_hash = anticheat_mix_u32(session->rolling_integrity_hash,
                                                        simulation_tick);
    session->rolling_integrity_hash = anticheat_mix_u32(
        session->rolling_integrity_hash,
        (uint32_t)((debugger_present ? 1u : 0u) + (uint32_t)code_integrity_violations * 5u +
                   (uint32_t)memory_map_anomalies * 7u + (uint32_t)timing_anomalies * 11u));

    *out_score = session->score;
    *out_flagged = compute_flagged(session);
    return BANANA_OK;
}

int banana_core_anticheat_submit_driver_telemetry(const char *session_id,
                                                  uint32_t heartbeat_sequence,
                                                  int suspicious_ring12_driver_count,
                                                  int unsigned_driver_count,
                                                  int hidden_module_count,
                                                  int *out_score,
                                                  int *out_flagged)
{
    BananaAnticheatSession *session = NULL;
    int delta = 0;

    if (!session_id || !session_id[0] || !out_score || !out_flagged)
        return BANANA_INVALID_ARGUMENT;

    if (suspicious_ring12_driver_count < 0 || unsigned_driver_count < 0 ||
        hidden_module_count < 0)
        return BANANA_INVALID_ARGUMENT;

    session = find_session(session_id, 1);
    if (!session)
        return BANANA_INTERNAL_ERROR;

    if (heartbeat_sequence <= session->last_driver_heartbeat)
        delta += 2;
    else if (session->last_driver_heartbeat > 0 &&
             heartbeat_sequence - session->last_driver_heartbeat > 8)
        delta += 1;

    delta += suspicious_ring12_driver_count * 4;
    delta += unsigned_driver_count * 2;
    delta += hidden_module_count * 3;

    if (delta == 0 && session->score > 0)
        session->score -= 1;
    else
        session->score += delta;

    if (session->score < 0)
        session->score = 0;

    if (heartbeat_sequence > session->last_driver_heartbeat)
        session->last_driver_heartbeat = heartbeat_sequence;

    session->rolling_integrity_hash = anticheat_mix_u32(session->rolling_integrity_hash,
                                                        heartbeat_sequence);
    session->rolling_integrity_hash = anticheat_mix_u32(
        session->rolling_integrity_hash,
        (uint32_t)(suspicious_ring12_driver_count * 13 + unsigned_driver_count * 17 +
                   hidden_module_count * 19));

    *out_score = session->score;
    *out_flagged = compute_flagged(session);
    return BANANA_OK;
}

int banana_core_anticheat_get_session_status(const char *session_id,
                                             int *out_score,
                                             int *out_flagged,
                                             uint32_t *out_integrity_hash)
{
    BananaAnticheatSession *session = NULL;

    if (!session_id || !session_id[0] || !out_score || !out_flagged || !out_integrity_hash)
        return BANANA_INVALID_ARGUMENT;

    session = find_session(session_id, 0);
    if (!session)
        return BANANA_NOT_FOUND;

    *out_score = session->score;
    *out_flagged = compute_flagged(session);
    *out_integrity_hash = session->rolling_integrity_hash;
    return BANANA_OK;
}
