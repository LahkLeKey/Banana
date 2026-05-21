#include "anticheat_trust.h"
#include "../entities/anticheat_session.h"
#include <stdint.h>

BananaTrustState anticheat_trust_get_state(const char *session_id) {
    BananaAnticheatSession *session = anticheat_session_find(session_id, 0);
    if (!session) return BANANA_TRUST_NEW;
    return session->trust_state;
}

void anticheat_trust_update(const char *session_id, int event) {
    BananaAnticheatSession *session = anticheat_session_find(session_id, 1);
    if (!session) return;
    // Trust progression and decay (see: Trust Lifecycle section)
    // Event codes: 1=good_behavior, 2=challenge_success, 3=anomaly, 4=challenge_fail, 5=corroborated_evidence
    switch (event) {
        case 1: // good_behavior
            if (session->trust_state == BANANA_TRUST_NEW)
                session->trust_state = BANANA_TRUST_ESTABLISHED;
            else if (session->trust_state == BANANA_TRUST_ELEVATED_OBSERVATION)
                session->trust_state = BANANA_TRUST_ESTABLISHED;
            else if (session->trust_state == BANANA_TRUST_SUSPICIOUS)
                session->trust_state = BANANA_TRUST_ELEVATED_OBSERVATION;
            break;
        case 2: // challenge_success
            if (session->trust_state == BANANA_TRUST_SUSPICIOUS)
                session->trust_state = BANANA_TRUST_ELEVATED_OBSERVATION;
            break;
        case 3: // anomaly
            if (session->trust_state == BANANA_TRUST_ESTABLISHED)
                session->trust_state = BANANA_TRUST_ELEVATED_OBSERVATION;
            else if (session->trust_state == BANANA_TRUST_ELEVATED_OBSERVATION)
                session->trust_state = BANANA_TRUST_SUSPICIOUS;
            else if (session->trust_state == BANANA_TRUST_SUSPICIOUS)
                session->trust_state = BANANA_TRUST_RESTRICTED;
            break;
        case 4: // challenge_fail
            session->trust_state = BANANA_TRUST_QUARANTINED;
            break;
        case 5: // corroborated_evidence
            session->trust_state = BANANA_TRUST_FLAGGED;
            break;
        default:
            break;
    }
}