#include "anticheat_session.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#define BANANA_ANTICHEAT_MAX_SESSIONS 256
#define BANANA_ANTICHEAT_SESSION_ID_MAX 64

static BananaAnticheatSession s_sessions[BANANA_ANTICHEAT_MAX_SESSIONS];

BananaAnticheatSession *anticheat_session_find(const char *session_id, int create) {
    if (!session_id) return 0;
    unsigned hash = 0;
    for (const char *p = session_id; *p; ++p) hash = hash * 31 + (unsigned char)*p;
    unsigned start = hash % BANANA_ANTICHEAT_MAX_SESSIONS;
    for (unsigned i = 0; i < BANANA_ANTICHEAT_MAX_SESSIONS; ++i) {
        unsigned idx = (start + i) % BANANA_ANTICHEAT_MAX_SESSIONS;
        BananaAnticheatSession *slot = &s_sessions[idx];
        if (slot->in_use && strncmp(slot->session_id, session_id, BANANA_ANTICHEAT_SESSION_ID_MAX-1) == 0)
            return slot;
        if (!slot->in_use && create) {
            memset(slot, 0, sizeof(*slot));
            slot->in_use = 1;
            snprintf(slot->session_id, BANANA_ANTICHEAT_SESSION_ID_MAX, "%s", session_id);
            return slot;
        }
    }
    return 0;
}

int anticheat_session_tamper_detected(const char *session_id) {
    (void)session_id;
    // TODO: Integrate with telemetry, return 1 if tamper detected
    return 0;
}

int anticheat_session_is_clock_valid(const char *session_id, uint32_t client_time, uint32_t server_time) {
    (void)session_id;
    // Return 1 if client time is within allowed skew of server time
    if (client_time > server_time + 1000 || client_time + 1000 < server_time)
        return 0;
    return 1;
}